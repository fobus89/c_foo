package main

import (
	"strconv"
	"strings"
)

func (l *Lexer) ReadString(symbol rune) token.TokenType {
	var (
		builder          strings.Builder
		hasInterpolation bool
	)

	// Пропускаем открывающую кавычку
	if !l.MatchAndNext(symbol) {
		// Возвращаем ошибку если нет открывающей кавычки
		return token.NewTokenType(token.ILLEGAL, "ILLEGAL", l.line, l.col)
	}

	var tokens []token.TokenType

	// Backtick — raw string: никаких escape-последовательностей, никакой интерполяции
	if symbol == '`' {
		for l.HasNext() {
			if l.MatchAndNext('`') {
				break
			}
			builder.WriteRune(l.Next())
		}
		return token.NewTokenType(token.STRING, builder.String(), l.line, l.col)
	}

	// Triple-quoted string: """..."""  — multiline, no interpolation, supports \n \t etc.
	if symbol == '"' && l.MatchAll('"', '"') {
		l.NextN(2) // consume the two extra '"'
		for l.HasNext() {
			if l.MatchAll('"', '"', '"') {
				l.NextN(3)
				break
			}
			ch := l.Next()
			if ch == '\\' && l.HasNext() {
				switch esc := l.Next(); esc {
				case 'n':
					builder.WriteByte('\n')
				case 't':
					builder.WriteByte('\t')
				case 'r':
					builder.WriteByte('\r')
				case '\\':
					builder.WriteByte('\\')
				case '"':
					builder.WriteByte('"')
				default:
					builder.WriteRune('\\')
					builder.WriteRune(esc)
				}
			} else {
				builder.WriteRune(ch)
			}
		}
		return token.NewTokenType(token.STRING, builder.String(), l.line, l.col)
	}

	for l.HasNext() {
		// Обработка экранированных символов
		// пропускаем \
		if l.MatchAndNext('\\') {
			// Добавляем экранированный символ в буфер
			switch ch := l.Next(); ch {
			case 'n':
				builder.WriteByte('\n')
			case 't':
				builder.WriteByte('\t')
			case 'r':
				builder.WriteByte('\r')
			case '\\':
				builder.WriteByte('\\')
			case '`', '"', symbol:
				builder.WriteRune(symbol)
			case '{':
				builder.WriteRune('{')
			case '}':
				builder.WriteRune('}')
			case 'u':
				// \u{XXXX} — Unicode code point
				if l.MatchAndNext('{') {
					var hexBuf strings.Builder
					for l.HasNext() && !l.Match('}') {
						hexBuf.WriteRune(l.Next())
					}
					l.MatchAndNext('}')
					if cp, err := strconv.ParseInt(hexBuf.String(), 16, 32); err == nil {
						builder.WriteRune(rune(cp))
					}
				} else {
					builder.WriteRune('u')
				}
			case 'x':
				// \xNN — hex byte
				var hexBuf strings.Builder
				for i := 0; i < 2 && l.HasNext(); i++ {
					hexBuf.WriteRune(l.Next())
				}
				if b, err := strconv.ParseInt(hexBuf.String(), 16, 32); err == nil {
					builder.WriteRune(rune(b))
				} else {
					builder.WriteString("\\x" + hexBuf.String())
				}
			default:
				builder.WriteRune(ch)
			}
			continue
		}

		// Конец строки
		if l.MatchAndNext(symbol) {
			// Добавляем последний кусок строки если есть
			if builder.Len() > 0 {
				tokens = append(tokens, token.NewTokenType(token.STRING, builder.String(), l.line, l.col))
			}
			break
		}

		// Проверка на интерполяцию {...}
		if l.MatchAndNext('{') {
			hasInterpolation = true

			// Сохраняем накопленную строку перед интерполяцией
			if builder.Len() > 0 {
				tokens = append(tokens, token.NewTokenType(token.STRING, builder.String(), l.line, l.col))
				builder.Reset()
			}

			// Читаем выражение внутри {}
			braceCount := 1
			for l.HasNext() && braceCount > 0 {
				t := l.Token()

				if t.Token == token.LBRACE {
					braceCount++
				} else if t.Token == token.RBRACE {
					braceCount--
					if braceCount == 0 {
						break
					}
				}

				// Парсим токены внутри интерполяции
				tokens = append(tokens, t)
			}
		} else {
			// Обычный символ строки
			builder.WriteRune(l.Next())
		}
	}

	// Если нет интерполяции и это простая строка
	if !hasInterpolation {
		if builder.Len() == 0 {
			return token.NewTokenType(token.STRING, "", l.line, l.col)
		}
		return token.NewTokenType(token.STRING, builder.String(), l.line, l.col)
	}

	l.tokens = append(l.tokens, token.NewTokenType(token.STRING_FORMAT_START, "STRING_FORMAT_START", l.line, l.col))

	l.tokens = append(l.tokens, tokens...)

	return token.NewTokenType(token.STRING_FORMAT_END, "STRING_FORMAT_END", l.line, l.col)
}
