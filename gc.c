#include "myLisp.h"

LVALUE* make_obj(VALUE root)
{
  int i,j;
  LVALUE *r;
  LVALUE *lastfree = freelist;


  if((!NIL_P(root)) && (freelist == NULL))
    {
      gc(root);  
    }  

  if(freelist == NULL)
    {
      heap.len += HEAP_GROW;
      if((heap.slots = (Heapslot**)realloc(heap.slots, sizeof(Heapslot*) * heap.len)) == NULL)
	{
	  fprintf(stderr, "memory allocate error\n");
	  exit(1);
	}

      for(i = 0; i < HEAP_GROW; i++)
	{
	  if((heap.slots[(heap.len - HEAP_GROW) + i] = (Heapslot*)malloc(sizeof(Heapslot))) == NULL)
	    {
	      fprintf(stderr, "memory allocate error\n");
	      exit(1);
	    }
	}
      

      for(i = 0; i < HEAP_GROW; i++)
	{
	  for(j = 0; j < SLOT_SIZE; j++)
	    {
	      heap.slots[(heap.len - HEAP_GROW) + i]->values[j].u.basic.type = FREE;
	      heap.slots[(heap.len - HEAP_GROW) + i]->values[j].u.basic.gc_mark = 0;
	      heap.slots[(heap.len - HEAP_GROW) + i]->values[j].u.free.next = NULL;
	      if(freelist == NULL)
		{
		  freelist = (heap.slots[(heap.len - HEAP_GROW) + i]->values + j);
		  lastfree = freelist;
		}
	      else
		{
		  lastfree->u.free.next = (heap.slots[(heap.len - HEAP_GROW) + i]->values + j);
		  lastfree = lastfree->u.free.next;
		}
	    }
	}
     }

  r = freelist;
  freelist = freelist->u.free.next;

  assert(r->u.basic.type == FREE);
  return r;
}

void gc(VALUE root)
{
  int i,j;
  LVALUE *slot;
  LVALUE *lastfree = NULL;
  
  freelist = NULL;

  recursive_mark((LVALUE*)root);

  for(i = 0; i < heap.len; i++)
    {
      for(j = 0; j < SLOT_SIZE; j++)
	{
	  slot = (heap.slots[i]->values) + j;
	  if(slot->u.basic.gc_mark == 0)
	    {
	      if(SYMBOL_P(slot)) free(slot->u.symbol.symbol);
	      slot->u.basic.type = FREE;
	      if(lastfree == NULL)
		{
		  freelist = slot;
		  lastfree = slot;
		}
	      else
		{
		  lastfree->u.free.next = slot;
		  lastfree = slot;
		}
	    }
	  else
	    {
	      slot->u.basic.gc_mark = 0;
	    }
	}
    }
}

void recursive_mark(LVALUE *v)
{
  if(DIRECTVAL_P(v) || v->u.basic.gc_mark == 1) return;

  v->u.basic.gc_mark = 1;

  if(SYMBOL_P(v)) return;
  else if(CLOSURE_P(v) || MACRO_P(v))
    {
      recursive_mark((LVALUE*)v->u.closure.params);
      recursive_mark((LVALUE*)v->u.closure.e);
      return;
    }
  else if(PAIR_P(v))
    {
      recursive_mark((LVALUE*)CAR(v));
      recursive_mark((LVALUE*)CDR(v));
      return;
    }
  
  fprintf(stderr, "recursive mark error\n");
  exit(1);
}
