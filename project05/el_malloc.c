
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "el_malloc.h"

////////////////////////////////////////////////////////////////////////////////
// Global control functions

// Global control variable for the allocator. Must be initialized in
// el_init().
el_ctl_t el_ctl = {};

// Create an initial block of memory for the heap using
// malloc(). Initialize the el_ctl data structure to point at this
// block. Initialize the lists in el_ctl to contain a single large
// block of available memory and no used blocks of memory.
int el_init(int max_bytes){
  void *heap = malloc(max_bytes);
  if(heap == NULL){
    fprintf(stderr,"el_init: malloc() failed in setup\n");
    exit(1);
  }

  el_ctl.heap_bytes = max_bytes; // make the heap as big as possible to begin with
  el_ctl.heap_start = heap;      // set addresses of start and end of heap
  el_ctl.heap_end   = PTR_PLUS_BYTES(heap,max_bytes);

  if(el_ctl.heap_bytes < EL_BLOCK_OVERHEAD){
    fprintf(stderr,"el_init: heap size %ld to small for a block overhead %ld\n",
            el_ctl.heap_bytes,EL_BLOCK_OVERHEAD);
    return 1;
  }
 
  el_init_blocklist(&el_ctl.avail_actual);
  el_init_blocklist(&el_ctl.used_actual);
  el_ctl.avail = &el_ctl.avail_actual;
  el_ctl.used  = &el_ctl.used_actual;

  // established the first available block by filling in size in
  // block/foot and null links in head
  size_t size = el_ctl.heap_bytes - EL_BLOCK_OVERHEAD;
  el_blockhead_t *ablock = el_ctl.heap_start;
  ablock->size = size;
  ablock->state = EL_AVAILABLE;
  el_blockfoot_t *afoot = el_get_footer(ablock);
  afoot->size = size;
  el_add_block_front(el_ctl.avail, ablock);
  return 0;
}

// Cleaned up the heap area associated with the system which simply
// calls free() on the malloc'd block used as the heap.
void el_cleanup(){
  free(el_ctl.heap_start);
  el_ctl.heap_start = NULL;
  el_ctl.heap_end   = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Pointer arithmetic functions to access adjacent headers/footers

// Compute the address of the foot for the given head which is at a
// higher address than the head.
el_blockfoot_t *el_get_footer(el_blockhead_t *head){
  size_t size = head->size;
  el_blockfoot_t *foot = PTR_PLUS_BYTES(head, sizeof(el_blockhead_t) + size);
  return foot;
}

// REQUIRED
// Compute the address of the head for the given foot which is at a
// lower address than the foot.
el_blockhead_t *el_get_header(el_blockfoot_t *foot){

  return PTR_MINUS_BYTES(foot, sizeof(el_blockhead_t) + foot->size);

}

// Return a pointer to the block that is one block higher in memory
// from the given block.  This should be the size of the block plus
// the EL_BLOCK_OVERHEAD which is the space occupied by the header and
// footer. DOES NOT follow next pointer, looks in adjacent memory.
el_blockhead_t *el_block_above(el_blockhead_t *block){
  el_blockhead_t *higher =
    PTR_PLUS_BYTES(block, block->size + EL_BLOCK_OVERHEAD);
  if((void *) higher >= (void*) el_ctl.heap_end){
    return NULL;
  }
  else{
    return higher;
  }
}

// REQUIRED
// Return a pointer to the block that is one block lower in memory
// from the given block.  Uses the size of the preceding block found
// in its foot. DOES NOT follow next pointer, looks in adjacent
// memory.
el_blockhead_t *el_block_below(el_blockhead_t *block){
  if((void *) PTR_MINUS_BYTES(block, sizeof(el_blockfoot_t)) <= el_ctl.heap_start){
    return NULL;
  }

  return el_get_header(PTR_MINUS_BYTES(block, sizeof(el_blockfoot_t)));

  
}

////////////////////////////////////////////////////////////////////////////////
// Block list operations

// Print an entire blocklist. The format appears as follows.
//
// blocklist{length:      5  bytes:    566}
//   [  0] head @    618 {state: u  size:    200}  foot @    850 {size:    200}
//   [  1] head @    256 {state: u  size:     32}  foot @    320 {size:     32}
//   [  2] head @    514 {state: u  size:     64}  foot @    610 {size:     64}
//   [  3] head @    452 {state: u  size:     22}  foot @    506 {size:     22}
//   [  4] head @    168 {state: u  size:     48}  foot @    248 {size:     48}
//   index        offset        a/u                       offset
//
// Note that the '@ offset' column is given from the starting heap
// address (el_ctl->heap_start) so it should be run-independent.
void el_print_blocklist(el_blocklist_t *list){
  printf("blocklist{length: %6lu  bytes: %6lu}\n", list->length,list->bytes);
  el_blockhead_t *block = list->beg;
  int i = 0;
  for(; i<list->length; i++){
    printf("  ");
    block = block->next;
    printf("[%3d] head @ %6lu ", i,PTR_MINUS_PTR(block,el_ctl.heap_start));
    printf("{state: %c  size: %6lu}", block->state,block->size);
    el_blockfoot_t *foot = el_get_footer(block);
    printf("  foot @ %6lu ", PTR_MINUS_PTR(foot,el_ctl.heap_start));
    printf("{size: %6lu}", foot->size);
    printf("\n");
  }
}

// Print out basic heap statistics. This shows total heap info along
// with the Available and Used Lists. The output format resembles the following.
//
// HEAP STATS
// Heap bytes: 1024
// AVAILABLE LIST: blocklist{length:      3  bytes:    458}
//   [  0] head @    858 {state: a  size:    126}  foot @   1016 {size:    126}
//   [  1] head @    328 {state: a  size:     84}  foot @    444 {size:     84}
//   [  2] head @      0 {state: a  size:    128}  foot @    160 {size:    128}
// USED LIST: blocklist{length:      5  bytes:    566}
//   [  0] head @    618 {state: u  size:    200}  foot @    850 {size:    200}
//   [  1] head @    256 {state: u  size:     32}  foot @    320 {size:     32}
//   [  2] head @    514 {state: u  size:     64}  foot @    610 {size:     64}
//   [  3] head @    452 {state: u  size:     22}  foot @    506 {size:     22}
//   [  4] head @    168 {state: u  size:     48}  foot @    248 {size:     48}
void el_print_stats(){
  printf("HEAP STATS\n");
  printf("Heap bytes: %lu\n",el_ctl.heap_bytes);
  printf("AVAILABLE LIST: ");
  el_print_blocklist(el_ctl.avail);
  printf("USED LIST: ");
  el_print_blocklist(el_ctl.used);
}

// Initialize the specified list to be empty. Sets the beg/end
// pointers to the actual space and initializes those data to be the
// ends of the list.  Initializes length and size to 0.
void el_init_blocklist(el_blocklist_t *list){
  list->beg        = &(list->beg_actual); 
  list->beg->state = EL_BEGIN_BLOCK;
  list->beg->size  = EL_UNINITIALIZED;
  list->end        = &(list->end_actual); 
  list->end->state = EL_END_BLOCK;
  list->end->size  = EL_UNINITIALIZED;
  list->beg->next  = list->end;
  list->beg->prev  = NULL;
  list->end->next  = NULL;
  list->end->prev  = list->beg;
  list->length     = 0;
  list->bytes      = 0;
}  

// REQUIRED
// Add to the front of list; links for block are adjusted as are links
// within list.  Length is incremented and the bytes for the list are
// updated to include the new block's size and its overhead.
void el_add_block_front(el_blocklist_t *list, el_blockhead_t *block){
  block->prev = list->beg;
  block->next = list->beg->next;
  block->prev->next = block;
  block->next->prev = block;
  list->length = list->length + 1;
  list->bytes  =  list->bytes + block->size + EL_BLOCK_OVERHEAD;
}

// REQUIRED
// Unlink block from the list it is in which should be the list
// parameter.  Updates the length and bytes for that list including
// the EL_BLOCK_OVERHEAD bytes associated with header/footer.
void el_remove_block(el_blocklist_t *list, el_blockhead_t *block){
  block->prev->next = block->next;
  block->next->prev = block->prev;
  list->length -= 1;
  list->bytes = list->bytes - block->size - EL_BLOCK_OVERHEAD;
}

////////////////////////////////////////////////////////////////////////////////
// Allocation-related functions

// REQUIRED
// Find the first block in the available list with block size of at
// least (size+EL_BLOCK_OVERHEAD). Overhead is accounted so this
// routine may be used to find an available block to split: splitting
// requires adding in a new header/footer. Returns a pointer to the
// found block or NULL if no of sufficient size is available.
el_blockhead_t *el_find_first_avail(size_t size) {
  el_blockhead_t *head = el_ctl.avail->beg->next;
  while ( head != el_ctl.avail->end  && head->size < (size + EL_BLOCK_OVERHEAD) ){
    head = head->next;
  }

  if (head == el_ctl.avail->end) {
    return NULL;
  } 
    el_remove_block(el_ctl.avail, head);
    el_add_block_front(el_ctl.avail, el_split_block(head, size));

    return head;
  
}


// REQUIRED
// Set the pointed to block to the given size and add a footer to
// it. Creates another block above it by creating a new header and
// assigning it the remaining space. Ensures that the new block has a
// footer with the correct size. Returns a pointer to the newly
// created block while the parameter block has its size altered to
// parameter size. Does not do any linking of blocks.  If the
// parameter block does not have sufficient size for a split (at least
// new_size + EL_BLOCK_OVERHEAD for the new header/footer) makes no
// changes and returns NULL.
el_blockhead_t *el_split_block(el_blockhead_t *block, size_t new_size){

    if(block->size < (new_size + EL_BLOCK_OVERHEAD)){
      return NULL;
    }
    size_t size_left = block->size - new_size - EL_BLOCK_OVERHEAD;
    el_blockhead_t * new_head = PTR_PLUS_BYTES(block, new_size + EL_BLOCK_OVERHEAD);
    new_head->size = size_left;
    new_head->state = EL_AVAILABLE;
    el_get_footer(new_head)->size = size_left;
    block->size = new_size;
    el_get_footer(block)->size = new_size;
    return new_head;
  
}

// REQUIRED
// Return pointer to a block of memory with at least the given size
// for use by the user.  The pointer returned is to the usable space,
// not the block header. Makes use of el_allocate_block() to find a
// suitable block. Returns NULL if no space is available.
void *el_malloc(size_t nbytes){
  el_blockhead_t * head = el_find_first_avail(nbytes);
  if(head == NULL){
    return NULL;
  }
  head->state = EL_USED;
  el_add_block_front(el_ctl.used, head);
  return PTR_PLUS_BYTES(head, sizeof(el_blockhead_t));
}

////////////////////////////////////////////////////////////////////////////////
// De-allocation/free() related functions

// REQUIRED
// Attempt to merge the block lower with the next block in
// memory. Does nothing if lower is null or not EL_AVAILABLE and does
// nothing if the next higher block is null (because lower is the last
// block) or not EL_AVAILABLE.  Otherwise, locates the next block with
// el_block_above() and merges these two into a single block. Adjusts
// the fields of lower to incorporate the size of higher block and the
// reclaimed overhead. Adjusts footer of higher to indicate the two
// blocks are merged.  Removes both lower and higher from the
// available list and re-adds lower to the front of the available
// list.
void el_merge_block_with_above(el_blockhead_t *lower){
  if(lower != NULL && lower->state == EL_AVAILABLE){

    el_blockhead_t * next_header = el_block_above(lower);

    if(next_header != NULL && next_header->state == EL_AVAILABLE){

      el_remove_block(el_ctl.avail, lower);
      el_remove_block(el_ctl.avail, next_header);

      lower->size = lower->size + next_header->size + EL_BLOCK_OVERHEAD;
      el_get_footer(lower)->size = lower->size;

      el_add_block_front(el_ctl.avail, lower);

      el_merge_block_with_above(lower);
    }

  }
}

// REQUIRED
// Free the block pointed to by the give ptr.  The area immediately
// preceding the pointer should contain an el_blockhead_t with information
// on the block size. Attempts to merge the free'd block with adjacent
// blocks using el_merge_block_with_above().
void el_free(void *ptr){
  el_blockhead_t * head = PTR_MINUS_BYTES(ptr, sizeof(el_blockhead_t));
  el_remove_block(el_ctl.used, head);

  head->state = EL_AVAILABLE;
  el_add_block_front(el_ctl.avail, head);

  el_blockhead_t * lower = el_block_below(head);
  while(lower != NULL && lower->state == EL_AVAILABLE) {
    head = lower;
    lower = el_block_below(lower);
  }
  el_merge_block_with_above(head);
}
