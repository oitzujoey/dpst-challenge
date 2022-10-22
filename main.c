#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

#define DOTIMES(I, TOP) for (ptrdiff_t I = 0; (size_t)I < (TOP); I++)
#define LSTR(value) value, sizeof(value) - 1

typedef enum {
	ERR_OK,
	ERR_PARSE_FAILED
} error_t;

typedef struct cons_s {
	size_t length;
	char key[4];
	const struct cons_s *cdr;
	bool free;
} cons_t;

#define HEAP_SIZE ((size_t) 100)
cons_t heap[HEAP_SIZE];
const cons_t *dictionary = NULL;

void heap_init(void) {
	DOTIMES(i, HEAP_SIZE) {
		heap[i].free = true;
	}
}

// Returns NULL on error.
cons_t *getFreeCons(void) {
	cons_t *freeCons = NULL;
	DOTIMES(i, HEAP_SIZE) {
		if (heap[i].free) {
			freeCons = &heap[i];
			freeCons->free = false;
			break;
		}
	}
	return freeCons;
}

cons_t *cons(const char* key, const size_t length, const cons_t *cdr) {
	cons_t *cons = getFreeCons();
	if (cons == NULL) return cons;
	DOTIMES(i, (length <= 4) ? length : 4) {
		cons->key[i] = key[i];
	}
	cons->length = length;
	cons->cdr = cdr;
	return cons;
}

void printCons(const cons_t *cons) {
	printf("(");
	if (cons != NULL) {
		DOTIMES(j, cons->length) {
			putchar(cons->key[j]);
		}
		printf(" . ");
		printCons(cons->cdr);
	}
	printf(")");
}

size_t list_length_helper(const cons_t *list, const size_t length) {
	if (list == NULL) return length;
	else return list_length_helper(list->cdr, length + 1);
}

size_t list_length(const cons_t *list) {
	return list_length_helper(list, 0);
}

void eatWhitespace(const char *source, const size_t length, ptrdiff_t *index) {
	while ((*index < length) && isspace(source[*index])) (*index)++;
}

error_t parseCommand(const char *source, const size_t length, ptrdiff_t *index) {
	(void) source;
	(void) length;
	(void) index;
	return ERR_PARSE_FAILED;
}

typedef struct {
	const char *string;
	size_t length;
} token_t;

void printToken(const token_t token) {
	DOTIMES(i, token.length) {
		putchar(token.string[i]);
	}
}

const token_t parseStringToken(const char *source, const size_t length, ptrdiff_t *index) {
	const char *token = &source[*index];
	size_t token_length = 0;
	while ((*index < length) && !isspace(source[*index])) {
		(*index)++;
		token_length++;
	}
	return (token_t) {.string = token, .length = token_length};
}

const cons_t *parseString(const cons_t *list, const char *source, const size_t length, ptrdiff_t *index) {
	token_t token;
	const cons_t *c = list;
	token = parseStringToken(source, length, index);
	while (token.length > 0) {
		// Build the token list.
		c = cons(token.string, token.length, c);
		// Get another token.
		/**/ eatWhitespace(source, length, index);
		token = parseStringToken(source, length, index);
	}
	return c;
}

// Do something like combinator parsing, but not quite.
const cons_t *parse(const cons_t *list, const char *source, const size_t length) {
	int e = 0;
	ptrdiff_t index = 0;
	const cons_t *c = list;
	/**/ eatWhitespace(source, length, &index);
	e = parseCommand(source, length, &index);
	if (e == ERR_PARSE_FAILED) {
		c = parseString(c, source, length, &index);
	}
	return c;
}

typedef int (*sort_callback_t)(const cons_t *left, const cons_t *right);

// Bubble sort, or something just as bad, because no speed or memory requirements.
const cons_t *sort_helper(const cons_t *list, const cons_t *cdr, sort_callback_t compare) {
	if (cdr == NULL) return cons(list->key, list->length, NULL);
	int result = compare(list, cdr);
	if (result > 0) {
		return cons(list->key, list->length, sort_helper(cons(cdr->key, cdr->length, NULL), cdr->cdr, compare));
	}
	else {
		return cons(cdr->key, cdr->length, sort_helper(cons(list->key, list->length, NULL), cdr->cdr, compare));
	}
}

const cons_t *sort(const cons_t *list, sort_callback_t compare) {
	const cons_t *c = list;
	const size_t length = list_length(list);
	DOTIMES(i, length) {
		c = sort_helper(c, c->cdr, compare);
	}
	return c;
}

int dict_elt_cmp(const cons_t *left, const cons_t *right) {
	if (left->length < right->length) {
		return 1;
	}
	else if (left->length > right->length) {
		return -1;
	}
	else {
		return 0;
	}
}

int main(int argc, char *argv[]) {
	/**/ heap_init();
	dictionary = parse(dictionary, LSTR("a bb"));
	dictionary = parse(dictionary, LSTR("ccc dddd"));
	printCons(dictionary);
	puts("");
	dictionary = sort(dictionary, dict_elt_cmp);
	printCons(dictionary);
	puts("");
	return 0;
}
