#include <stdio.h>
#include <stdlib.h>
#include "gc.h"
#include <time.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

struct heap{
  void * memory;
  size_t size;
  bool unsafe_stack;
  float gc_threshold;
  int number_of_pages;
  page_t * pages[];
};


struct page{
  void * start;
  void * bump;
  int size;
};


struct test_struct{
  int first;
  void * second;
  int third;
};

void 
test_h_init ()
{
  time_t t;
  srand( (unsigned) time(&t));

  for (int i = 1; i <= 42; i++) { 
    int int_rand = (rand() % 20) + 1;
    int test_size = 2048 * int_rand;
    //printf("\nTest size: %d\n", test_size);

    heap_t *test_h_init_heap = h_init(test_size, true, 1);
    CU_ASSERT(test_h_init_heap != NULL);
    CU_ASSERT(test_h_init_heap->size == (size_t)test_size);
    h_delete(test_h_init_heap);
  }
}


void 
test_pages ()
{
  time_t t;
  srand( (unsigned) time(&t));

  for (int i = 1; i <= 420; i++) {
    int rand_int = (rand() %20) + 1;
    int test_size = 2048 * rand_int;
    heap_t *test_pages_heap = h_init(test_size, true, 1);
    void * memory_end = test_pages_heap->memory + test_size;
    void * pages_end = (test_pages_heap->memory + test_size + (sizeof(page_t) * test_pages_heap->number_of_pages) );
    for(int j = 0; j < rand_int; j++){
      void * ptr = (void *)test_pages_heap->pages[j];
      CU_ASSERT( ptr >= memory_end && ptr < pages_end);
      //printf("\nMemory end:%lu\nPointer:   %lu\nPages end: %lu\n",
      //       (long unsigned)memory_end, (long unsigned)ptr, (long unsigned)pages_end);
    }

    //printf("\nMem start:  %p  \n", get_memory(test_pages_heap));  
    //printf("First page: %p \n", get_page_start(test_pages_heap->pages[0]));
    //printf("Second page:%p\n", get_page_start(test_pages_heap->pages[1]));
    //printf("Third page: %p\n", get_page_start(test_pages_heap->pages[2]));
    //printf("Mem end:    %p  \n", get_memory(test_pages_heap) + test_size);
 
    h_delete(test_pages_heap);
  }
}


void
test_h_alloc ()
{
  int test_size = 6144;
  heap_t *test_h_alloc_heap = h_init(test_size, true, 1);

  void * ptr1 = h_alloc_struct(test_h_alloc_heap, "i");
  write_int_to_heap(ptr1, 6);

  void * ptr2 = h_alloc_struct(test_h_alloc_heap, "i");
  write_int_to_heap(ptr2, 9);

  void * ptr4 = h_alloc_data(test_h_alloc_heap, 2040);

  void * ptr5 = h_alloc_struct(test_h_alloc_heap, "i*i");
  struct test_struct ts = ( (struct test_struct) {1, malloc(4),3}); 
  *(struct test_struct *)ptr5 = ts;
  //printf("\nTest struct:  %d %p %d  \n", (*(struct test_struct *)ptr5).first, 
  //       (*(struct test_struct *)ptr5).second,
  //       (*(struct test_struct *)ptr5).third );



  void * ptr3 = h_alloc_struct(test_h_alloc_heap, "*");
  void * test_pointer = malloc(10);
  write_pointer_to_heap(&ptr3, test_pointer);

  //printf("\nTest pointer:  %p  \n", test_pointer);  
  //printf("Pointer 2    : %p \n", (void *)ptr2 );
  //printf("\nAvail: %lu \n", h_avail(test_h_alloc_heap));


  CU_ASSERT(*(int *) ptr1 == 6);
  CU_ASSERT(*(int *)ptr2 == 9);
  CU_ASSERT_PTR_EQUAL(test_pointer, ptr3);
  CU_ASSERT(ptr4 != NULL);
  CU_ASSERT(ptr5 != NULL);

  h_delete(test_h_alloc_heap);
  free(test_pointer);
}


void 
test_h_size()
{
  time_t t;
  srand( (unsigned) time(&t));

  for (int i = 1; i <= 42; i++) {

    int test_size = 2048;
    int rand_nr = rand() % 2000;
    heap_t *test_h_size_heap = h_init(test_size, true, 1);
    //printf("rand: %d\n",rand_nr);
    //printf("\nUsed: %lu\n", h_used(test_h_size_heap));
    //printf("\nAvail: %lu\n", h_avail(test_h_size_heap));
    h_alloc_data(test_h_size_heap, rand_nr);
    //printf("\nUsed after: %lu\n", h_used(test_h_size_heap));
    //printf("\nAvail after: %lu\n", h_avail(test_h_size_heap));
    
    CU_ASSERT(h_avail(test_h_size_heap) == ( (size_t) (test_size) - rand_nr) );
    CU_ASSERT(h_used(test_h_size_heap) == ( (size_t) rand_nr) );
    CU_ASSERT(h_size(test_h_size_heap) == ( (size_t) test_size) );
    h_delete(test_h_size_heap);  

  }
  
}





int
main (int argc, char *argv[])
{
  CU_pSuite suite1 = NULL;

  if (CU_initialize_registry() != CUE_SUCCESS)
    {
      return CU_get_error();
    }

  suite1 = CU_add_suite("Heap Test", NULL, NULL);

  if (
      (CU_add_test(suite1, "test_h_init()", test_h_init) == NULL) ||
      (CU_add_test(suite1, "test_page()", test_pages) == NULL) ||
      (CU_add_test(suite1, "test_h_alloc_struct/data()", test_h_alloc) == NULL) ||
      (CU_add_test(suite1, "test_h_size/avail/used()", test_h_size) == NULL)
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return CU_get_error();
}
