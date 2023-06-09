/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2001 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Program(RAP) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** 2001/11/19 23:18:37 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
#ifndef SIMPLE_QUEUE_H
#define SIMPLE_QUEUE_H

#include <toolsa/mem.h>

template <class T>
class SimpleQueue {
 public:

   // Constructor

   SimpleQueue()
   {
     _size = 0;   // Initialize the array
     _first = NULL;
     _last = NULL;
   }
  
   // Destructor

   ~SimpleQueue()
   {
     _data_node_t *next_list;
     
     while (_first != NULL)
     {
       next_list = _first->next;
       ufree((char *)_first->data);
       ufree((char *)_first);
       _first = next_list;
     }
   }
  
   // Insert a node at the end of the queue.

   void insert(T t)
   {
     _data_node_t *new_node = (_data_node_t *)umalloc(sizeof(_data_node_t));
     T *new_data = (T *)umalloc(sizeof(T));
     
     *new_data = t;
     
     // Always add at the back of the list.

     new_node->data = new_data;
     new_node->next = NULL;
     new_node->prev = _last;
     
     if (_first == NULL)
       _first = new_node;

     if (_last != NULL)
       _last->next = new_node;
     
     _last = new_node;
     
     _size++;
     
     return;
   }

   // Remove and return the first node in the queue.  Returns a
   // pointer to the data for the node.  This pointer must be
   // freed by the calling routine.

   T *remove()
   {
     T *data;
     _data_node_t *node = _first;
     
     // Check for an empty list

     if (node == NULL)
       return(NULL);
     
     data = node->data;
     
     // Update the list pointers

     _first = _first->next;
     if (_first == NULL)
       _last = NULL;
     
     // Update the list size

     _size--;
     
     // Free the node

     ufree(node);
     
     return(data);
   }
   
   // Returns the number of nodes in the queue.

   int size() { return _size; };
  
 private:

   typedef struct _data_node_s
   {
     T* data;
     struct _data_node_s *next;
     struct _data_node_s *prev;
   } _data_node_t;
   
   _data_node_t *_first;
   _data_node_t *_last;
   
   int _size;

};

#endif
