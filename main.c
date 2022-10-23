#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define DOTIMES(I, TOP) for (ptrdiff_t I = 0; (size_t)I < (TOP); I++)
#define LSTR(value) value, sizeof(value) - 1

/* "Cons" is just another name for a link in a list. Usually conses imply garbage collection, which is one reason I have
   used that name instead of "link". There is currently no garbage collection, but I will add that later. This has the
   advantage that it effectively gives me malloc while still only using static allocation, plus I don't have to worry
   about freeing memory anywhere other than the collector. */

typedef struct cons_s {
	size_t length;
	char key[4];
	const struct cons_s *cdr;
	bool free;
} cons_t;

#define HEAP_SIZE ((size_t) 1000)
cons_t heap[HEAP_SIZE];
const cons_t **root;

void heap_init(const cons_t **r) {
	root = r;
	DOTIMES(i, HEAP_SIZE) heap[i].free = true;
}

void trace(const cons_t *const_c) {
	if (const_c == NULL) return;
	// Remove const.
	cons_t *c = &heap[const_c - heap];
	c->free = false;
	// Note we are not doing cycle detection. There is no need for that.
	trace(c->cdr);
}

void garbageCollect(void) {
	// Mark ALL conses as free.
	DOTIMES(i, HEAP_SIZE) heap[i].free = true;
	// Mark all conses that can be reached from the root.
	trace(*root);
}

size_t freeConses(void) {
	size_t f = 0;
	DOTIMES(i, HEAP_SIZE) {
		if (heap[i].free) {
			f++;
		}
	}
	return f;
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
	// May not be safe to run GC here, so don't.
	if (freeCons == NULL) puts("Out of memory");
	return freeCons;
}

cons_t *cons(const char* key, const size_t length, const cons_t *cdr) {
	cons_t *c = getFreeCons();
	if (c == NULL) return c;
	DOTIMES(i, (length <= 4) ? length : 4) {
		c->key[i] = key[i];
	}
	// Store word length, not the length stored in `key`.
	c->length = length;
	c->cdr = cdr;
	return c;
}

#define CAR(c) c->key, c->length

void printCons(const cons_t *c) {
	printf("(");
	if (c != NULL) {
		size_t length = (c->length < 4) ? c->length : 4;
		DOTIMES(j, length) {
			putchar(c->key[j]);
		}
		printf(" . ");
		printCons(c->cdr);
	}
	printf(")");
}

// Tail recursive. Clang and GCC should be able to optimize this.
size_t list_length_helper(const cons_t *list, const size_t length) {
	if (list == NULL) return length;
	else return list_length_helper(list->cdr, length + 1);
}

size_t list_length(const cons_t *list) {
	return list_length_helper(list, 0);
}

const cons_t *deleteList(const cons_t *list) {
	(void) list;
	return NULL;
}

const cons_t *reverse(const cons_t *list) {
	if (list == NULL) return list;
	const cons_t *c = NULL;
	while (list != NULL) {
		c = cons(CAR(list), c);
		list = list->cdr;
	}
	return c;
}

const cons_t *elt(const cons_t *list, ptrdiff_t index) {
	if (list == NULL) return NULL;
	if (index < 0) return NULL;
	if (index == 0) return list;
	return elt(list->cdr, index - 1);
}

// Tail recursive
void printList_helper(const cons_t *list) {
	size_t length = (list->length < 4) ? list->length : 4;
	DOTIMES(j, length) {
		putchar(list->key[j]);
	}
	if (list->cdr != NULL) {
		putchar(' ');
		printList_helper(list->cdr);
	}
}

const cons_t *printList(const cons_t *list) {
	putchar('(');
	if (list != NULL) /**/ printList_helper(list);
	putchar(')');
	putchar('\n');
	return list;
}

/* Parse */

typedef const cons_t *(*parser_t)(const cons_t *, const char *, const size_t, ptrdiff_t *);

// Only works on ASCII characters.
bool isWordCharacter(const char c) {
	return ((('0' <= c) && (c <= '9'))
	        || (('a' <= c) && (c <= 'z'))
	        || (('A' <= c) && (c <= 'Z'))
	        || ('-' == c)
	        || ('\'' == c)
	        || ('_' == c));  // Might as well put "_" in because of identifiers in programming languages.
}

void eatWhitespace(const char *source, const size_t length, ptrdiff_t *index) {
	while ((*index < length) && !isWordCharacter(source[*index])) (*index)++;
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
	while ((*index < length) && isWordCharacter(source[*index])) {
		(*index)++;
		token_length++;
	}
	return (token_t) {.string = token, .length = token_length};
}

const cons_t *parseString(const cons_t *list, const char *source, const size_t length, ptrdiff_t *index) {
	token_t token;
	const cons_t *c = list;
	/**/ eatWhitespace(source, length, index);
	token = parseStringToken(source, length, index);
	while (token.length > 0) {
		// Build the token list.
		c = cons(token.string, token.length, c);
		if (c == NULL) return c;
		// Get another token.
		/**/ eatWhitespace(source, length, index);
		token = parseStringToken(source, length, index);
	}
	return reverse(c);
}

void printHelp(void);

const cons_t *parseHelp(const cons_t *list, const char *source, const size_t length, ptrdiff_t *index) {
	ptrdiff_t local_index = *index;
	if (local_index >= length) return NULL;
	if (source[local_index++] == 'h') {
		return cons(LSTR("?h"), NULL);
	}
	return NULL;
}

const cons_t *parseAdd(const cons_t *list, const char *source, const size_t length, ptrdiff_t *index) {
	ptrdiff_t local_index = *index;
	if (local_index >= length) return NULL;
	const cons_t *c = NULL;
	if (source[local_index++] == 'a') {
		c = cons(LSTR("?a"), parseString(list, source, length, &local_index));
		*index = local_index;
	}
	return c;
}

const cons_t *parseSearch(const cons_t *list, const char *source, const size_t length, ptrdiff_t *index) {
	ptrdiff_t local_index = *index;
	if (local_index >= length) return NULL;
	const cons_t *c = NULL;
	if (source[local_index] == 's') {
		local_index++;
		c = cons(LSTR("?s"), parseString(list, source, length, &local_index));
		*index = local_index;
	}
	else if (source[local_index] == 'S') {
		local_index++;
		c = cons(LSTR("?S"), parseString(list, source, length, &local_index));
		*index = local_index;
	}
	return c;
}

const cons_t *parseDelete(const cons_t *list, const char *source, const size_t length, ptrdiff_t *index) {
	ptrdiff_t local_index = *index;
	if (local_index >= length) return NULL;
	const cons_t *c = NULL;
	if (source[local_index++] == 'd') {
		c = cons(LSTR("?d"), NULL);
		*index = local_index;
	}
	return c;
}

const cons_t *parsePrint(const cons_t *list, const char *source, const size_t length, ptrdiff_t *index) {
	ptrdiff_t local_index = *index;
	if (local_index >= length) return NULL;
	const cons_t *c = NULL;
	if (source[local_index++] == 'p') {
		c = cons(LSTR("?p"), NULL);
		*index = local_index;
	}
	return c;
}

const cons_t *parsePrintFree(const cons_t *list, const char *source, const size_t length, ptrdiff_t *index) {
	ptrdiff_t local_index = *index;
	if (local_index >= length) return NULL;
	const cons_t *c = NULL;
	if (source[local_index++] == 'f') {
		c = cons(LSTR("?f"), NULL);
		*index = local_index;
	}
	return c;
}

const cons_t *parseQuit(const cons_t *list, const char *source, const size_t length, ptrdiff_t *index) {
	ptrdiff_t local_index = *index;
	if (local_index >= length) return NULL;
	const cons_t *c = NULL;
	if (source[local_index++] == 'q') {
		c = cons(LSTR("?q"), NULL);
		*index = local_index;
	}
	return c;
}

// I think this is something like combinator parsing, except it's procedural.
const cons_t *parseCommand(const cons_t *list, const char *source, const size_t length, ptrdiff_t *index) {
	ptrdiff_t local_index = *index;
	if (local_index >= length) return NULL;
	if (source[local_index++] != '?') return NULL;
	parser_t parsers[] = {parseHelp,
	                      parseAdd,
	                      parseSearch,
	                      parseDelete,
	                      parsePrint,
	                      parsePrintFree,
	                      parseQuit};
	const cons_t *c = NULL;
	DOTIMES(i, sizeof(parsers)/sizeof(parser_t)) {
		c = parsers[i](list, source, length, &local_index);
		if (c != NULL) break;
	}
	if (c != NULL) *index = local_index;
	return c;
}

const cons_t *parse(const cons_t *list, const char *source) {
	const size_t length = strlen(source);
	ptrdiff_t index = 0;
	const cons_t *c = list;
	c = parseCommand(c, source, length, &index);
	if (c == NULL) {
		c = cons(LSTR("?a"), parseString(list, source, length, &index));
	}
	return c;
}

/* Sort */

typedef int (*sort_callback_t)(const cons_t *left, const cons_t *right);

const cons_t *insertionSort(const cons_t *list, sort_callback_t compare) {
	size_t length = list_length(list);
	DOTIMES(i, length) {
		ptrdiff_t j = i;
		while (j > 0) {
			const cons_t *left_parent = elt(list, j - 2);
			const cons_t *left = NULL;
			if (left_parent == NULL) left = list;
			else left = left_parent->cdr;
			const cons_t *right = left->cdr;
			const cons_t *right_child = right->cdr;
			if (compare(left, right) < 0) break;
			if (left_parent != NULL) heap[left_parent - heap].cdr = right;
			else list = right;
			heap[right - heap].cdr = left;
			heap[left - heap].cdr = right_child;
			--j;
		}
	}
	return list;
}

const cons_t *sort(const cons_t *list, sort_callback_t compare) {
	return insertionSort(list, compare);
}

/* Evaluate */

char *readline(const char *prompt) {
#define BUFFER_SIZE ((size_t) 100)
	while (*prompt != '\0') putchar(*(prompt++));
	static char buffer[BUFFER_SIZE];
	ptrdiff_t index = 0;
	int in;
	while (index < (BUFFER_SIZE - 1)) {
		in = getchar();
		if ((in == EOF) || (in == '\n')) break;
		buffer[index] = in;
		index++;
	}
	buffer[index] = '\0';
	if (index == (BUFFER_SIZE - 1)) printf("Input truncated to %lu characters.\n", BUFFER_SIZE - 1);
	return buffer;
}

void printFree(void) {
	printf("Free: %lu\n", freeConses());
}

void printHelp(void) {
	printf("?h Print this help.\n"
	       "?s Search for word. Case insensitive.\n"
	       "?S Search for word. Case sensitive.\n"
	       "?a Add new words. Words are also added when a command is not explicitly given.\n"
	       "?d Delete list.\n"
	       "?f Print free memory.\n"
	       "?q Quit.\n");
}

const cons_t *search(const cons_t *list, const cons_t *query, bool case_sensitive) {
	if (query == NULL) return query;
	if (list == NULL) return list;
	if (list->length == query->length) {
		bool equal = true;
		size_t length = ((query->length > 4) ? 4 : query->length);
		DOTIMES(i, length) {
			char left = list->key[i];
			char right = query->key[i];
			if (!case_sensitive) left = toupper(left);
			if (!case_sensitive) right = toupper(right);
			if (left != right) {
				equal = false;
				break;
			}
		}
		if (equal) return list;
	}
	return search(list->cdr, query, case_sensitive);
}

// Sort's comparison function.
int compare_conses(const cons_t *left, const cons_t *right) {
	if ((left == NULL) || (right == NULL)) {
		if (left == NULL) return -1;
		else return 1;
	}
	size_t left_length = ((left->length > 4) ? 4 : left->length);
	size_t right_length = ((right->length > 4) ? 4 : right->length);
	size_t length = ((left_length > right_length) ? right_length : left_length);
	DOTIMES(i, length) {
		char difference = toupper(left->key[i]) - toupper(right->key[i]);
		if (difference != 0) return difference;
	}
	// If equal so far, then go off of lengths.
	return left->length - right->length;
}

// Accepts an alphabetically sorted list.
const cons_t *removeDups(const cons_t *list) {
	const cons_t *last = NULL;
	const cons_t *new = NULL;

	while (list != NULL) {
		if (0 != compare_conses(list, last)) {
			new = cons(CAR(list), new);
		}
		last = list;
		list = list->cdr;
	}
	return reverse(new);
}

const cons_t *merge(const cons_t *first, const cons_t *second) {
	if (second == NULL) return first;
	const cons_t *new = second;
	while (first != NULL) {
		new = cons(CAR(first), new);
		first = first->cdr;
	}
	return new;
}

bool run = true;

const cons_t *eval(const cons_t *dict, const cons_t *ast) {
	if (ast == NULL) return ast;
	if (ast->length == 2) {
		if (ast->key[0] == '?') {
			if (ast->key[1] == 'h') {
				/**/ printHelp();
				return dict;
			}
			else if (ast->key[1] == 'd') {
				// This deletes the list once GC is run.
				return NULL;
			}
			else if (ast->key[1] == 's') {
				const cons_t *result = search(dict, ast->cdr, false);
				if (result == NULL) return dict;
				size_t length = (result->length < 4) ? result->length : 4;
				DOTIMES(l, length) {
					putchar(result->key[l]);
				}
				putchar('\n');
				return dict;
			}
			else if (ast->key[1] == 'S') {
				const cons_t *result = search(dict, ast->cdr, true);
				if (result == NULL) return dict;
				size_t length = (result->length < 4) ? result->length : 4;
				DOTIMES(l, length) {
					putchar(result->key[l]);
				}
				putchar('\n');
				return dict;
			}
			else if (ast->key[1] == 'a') {
				return printList(removeDups(sort(merge(dict, ast->cdr), compare_conses)));
			}
			else if (ast->key[1] == 'p') {
				return printList(dict);
			}
			else if (ast->key[1] == 'f') {
				// GC so that we only print the true memory available.
				// GC should be safe here because we have made no changes to `dict`.
				garbageCollect();
				printFree();
				return dict;
			}
			else if (ast->key[1] == 'q') {
				run = false;
				return NULL;
			}
		}
	}
	return NULL;
}

void printWelcome(void) {
	/**/ printHelp();
}

const cons_t *dictionary = NULL;

int main(int argc, char *argv[]) {
	/**/ heap_init(&dictionary);
	/**/ printWelcome();
	while (run) {
		dictionary = eval(dictionary, parse(NULL, readline("> ")));
		garbageCollect();
	}
	return 0;
}
