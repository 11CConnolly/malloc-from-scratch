#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Abstraction where memory comes from
// Not being initialized, random data in it.
static uint8_t MEMORY_POOL[64];

// Object-like thing with keys and values to a single hole in memory
struct free_entry
{
    // Address at which the hole exists
    void *ptr;
    // Size of hole
    uint64_t size;
};

// typedef struct xxx allows me to say "free_entry_t" instead of struct "free_entry" everytime
typedef struct free_entry free_entry_t;

// Global list of free entries in memory, initing a structure, specify what's in the array
static free_entry_t FREE_LIST[64] = {
    (free_entry_t){
        .ptr = MEMORY_POOL,
        .size = 64},
    // {...}, {...}, ... and so on with more and more holes
};

// Track how many, begin with something in free list to allocate from
static uint64_t FREE_LIST_USED = 1;

// Debugging information
void print_free_list()
{
    printf("FREE LIST:\n");
    for (uint64_t i = 0; i < FREE_LIST_USED; i++)
    {
        free_entry_t *entry;
        entry = &FREE_LIST[i];
        printf(" %p (%llu)\n", entry->ptr, entry->size);
    }
}

free_entry_t *find_free_entry(size_t size)
{
    // Iterate through free list and find first free entry big enough
    for (uint64_t i = 0; i < FREE_LIST_USED; i++)
    {
        free_entry_t *entry;
        entry = &FREE_LIST[i];
        if (entry->size >= size)
        {
            return entry;
        }
    }

    abort();
}

// Void means you don't know what it is, it's nothing, memory address still
// Just like a 64 bit unsigned integer, these numbers have meanings
// size_t will always have the same size as computer
void *malloc(size_t size)
{
    size += 8;

    // Find entry to do allocation in
    free_entry_t *entry;
    entry = find_free_entry(size);

    // Distinguish between different pointers
    void *base_ptr;
    uint64_t *size_ptr;
    void *user_ptr;

    // Both memory addresses so can assign one to the other
    base_ptr = entry->ptr;
    size_ptr = base_ptr;
    user_ptr = base_ptr + 8;

    // Write 12 to memory address of a pointer, dereferences
    *size_ptr = size;

    // Adjust free entry to reflect we've used that region
    entry->ptr += size;
    entry->size -= size;

    print_free_list();

    return user_ptr;
}

// Before pointer that the user is given, we allocate x many bytes of size
// WHen that pointer is freed, step back x many bytes

// WHen we free bytes, need to create a new entry to say those bytes aren't currently used
void free(void *ptr)
{
    // // New entry
    // free_entry_t *entry;
    // entry = &FREE_LIST[FREE_LIST_USED];

    // entry-> ptr = something
    // // No way to know the size w/ metadata
    // entry->size = something

    // FREE_LIST_USED++;
}

int main()
{
    print_free_list();

    char *a;
    char *b;
    char *c;
    char *d;

    a = malloc(4);
    b = malloc(4);
    c = malloc(4);

    strcpy(a, "foo\0");
    strcpy(b, "bar\0");
    strcpy(c, "baz\0");

    printf("%p\n", a);
    printf("%p\n", b);
    printf("%p\n", c);

    // Be able to free memory once that previous memory has been
    free(d);
    d = malloc(4);

    printf("%p\n", d);
}