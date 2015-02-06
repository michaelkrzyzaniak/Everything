/*_--__-_--___-----_-------_------------_-_-----_-----_----------------------------------
|  \/  | |/ / |	  (_)_ __ | | _____  __| | |   (_)___| |_   ___	
| |\/| | ' /| |	  | | '_ \| |/ / _ \/ _` | |   | / __| __| / __|
| |  | | . \| |___| | |	| |   <	 __/ (_| | |___| \__ \ |_ | (__	
|_|  |_|_|\_\_____|_|_|	|_|_|\_\___|\__,_|_____|_|___/\__(_)___|

---------------------------------------------------------------------------------------*/
  
#include "List.h"
  
/*--------_----------_------------_--------_---------------_---_-------------------------
 _ __ _	_(_)_ ____ _| |_ ___   __| |___	__| |__	_ _ _ __ _| |_(_)___ _ _  ___
| '_ \ '_| \ V / _` |  _/ -_) /	_` / -_) _| / _` | '_/ _` |  _|	/ _ \ '	\(_-<
| .__/_| |_|\_/\__,_|\__\___| \__,_\___\__|_\__,_|_| \__,_|\__|_\___/_||_/__/
|_|    	       	       	       	       	       	       	       	       	     
---------------------------------------------------------------------------------------*/

typedef struct
{ 
  void*      prev;
  listData_t data;
  void*      next;
  listDataDeallocator_t deallocateData;
}listEntry_t;
  
//---------------------------------------------------------------------------------------
struct opaqueListStruct
{ 
  unsigned count;
  listEntry_t         *firstEntry;
  listEntry_t         *currentEntry;
  listEntry_t         *lastEntry;
};

//---------------------------------------------------------------------------------------
listEntry_t*      listNewEntry                ();
listEntry_t*      listEntryAtIndex            (List* self, unsigned index);
void              listRemoveEntryAtIndex      (List* self, unsigned index, BOOL shouldDeallocate);
void              listCleanEntry              (List* self, listEntry_t* entry);
//void              listDefaultDataDeallocator(listData_t data);

/*------------_---_----------------------------------------------------------------------
 _ _ ___ _  _| |_(_)_ _	 ___ ___
| '_/ _	\ || | 	_| | ' \/ -_|_-<
|_| \___/\_,_|\__|_|_||_\___/__/
---------------------------------------------------------------------------------------*/

listEntry_t* listNewEntry()
{
  return (listEntry_t*)malloc(sizeof(listEntry_t));
}      

//entryAtIndex---------------------------------------------------------------------------
listEntry_t* listEntryAtIndex(List* self, unsigned index)
{
  listEntry_t* thisEntry = self->firstEntry;
  if(thisEntry != NULL)
    {
      unsigned maxIndex = listCount(self)-1;
      if(index > maxIndex) index = maxIndex;
   
      while(index-- > 0)
        thisEntry = thisEntry->next;
    }
  return thisEntry;
}

//cleanEntry-----------------------------------------------------------------------------
void listCleanEntry(List* self, listEntry_t* entry)
{
  if(entry->deallocateData != NULL)
    entry->deallocateData(entry->data);
  //free(entry->data) if it was allocated with malloc;
}

//removeEntryAtIndex---------------------------------------------------------------------
void listRemoveEntryAtIndex(List* self, unsigned index, BOOL shouldDeallocate)
{
  if(index < listCount(self))
    {
      listEntry_t* thisEntry = listEntryAtIndex(self, index);
      if (shouldDeallocate) listCleanEntry(self, thisEntry);
      if(thisEntry->prev != NULL)
        ((listEntry_t*)thisEntry->prev)->next = thisEntry->next;
      else
        self->firstEntry = (listEntry_t*)thisEntry->next;
      if(thisEntry->next != NULL)
        ((listEntry_t*)thisEntry->next)->prev = thisEntry->prev;
      else self->lastEntry = thisEntry->prev;
      free(thisEntry);
      self->count--;
    }
}

//listCreate-----------------------------------------------------------------------------
List* listNew()
{
  List* self = calloc(1, sizeof(*self));

  return self;
}

//---------------------------------------------------------------------------------------
List* listShallowCopy(List* self)
{
  List* copy = listNew();
  
  if(copy != NULL)
    {
      listEntry_t* current = self->firstEntry;
      while(current != NULL)
        {
          listAppendData(copy, current->data, current->deallocateData);
          current = current->next;
        }
    }
  return copy;
}

//count----------------------------------------------------------------------------------
unsigned listCount(List* self)
{
  return self->count;
}

//appendData-----------------------------------------------------------------------------
void listAppendData(List* self, listData_t data, listDataDeallocator_t deallocator)
{
  listEntry_t* thisEntry = listNewEntry();
  if(thisEntry != NULL)
    {
      if(self->firstEntry == NULL) self->firstEntry = thisEntry; 
      listEntry_t* prevEntry = self->lastEntry;
      thisEntry->prev = prevEntry;
      if(prevEntry != NULL)prevEntry->next = thisEntry;
      self->lastEntry = thisEntry;
    
      thisEntry->next = NULL;
      thisEntry->data = data;
      thisEntry->deallocateData = deallocator;
      self->count++;
    }
}

//insertDataAtIndex----------------------------------------------------------------------
void listInsertDataAtIndex(List* self, listData_t data, unsigned index, listDataDeallocator_t deallocator)
{
  if(index >= listCount(self)) 
    listAppendData(self, data, deallocator);
  else
    {
      listEntry_t* thisEntry = listNewEntry();
      if(thisEntry != NULL)
        {
          listEntry_t* nextEntry = listEntryAtIndex(self, index);
          listEntry_t* prevEntry = nextEntry->prev;
          
          thisEntry->next     = nextEntry;
          thisEntry->prev = prevEntry;
          
          if(prevEntry != NULL)
            prevEntry->next = thisEntry;
          if(nextEntry != NULL)
	          nextEntry->prev = thisEntry;
          if(index == 0) 
	          self->firstEntry = thisEntry;
            
          thisEntry->data = data;
          thisEntry->deallocateData = deallocator;
          self->count++;
        }
    }
}

//removeDataAtIndex----------------------------------------------------------------------
void listRemoveDataAtIndex(List* self, unsigned index, BOOL shouldDeallocate)
{
  listRemoveEntryAtIndex(self, index, shouldDeallocate);
}

//dataAtIndex----------------------------------------------------------------------------
listData_t listDataAtIndex(List* self, unsigned index)
{
  return listEntryAtIndex(self, index)->data;
}

//replaceDataAtIndex---------------------------------------------------------------------
void listReplaceDataAtIndex(List* self, listData_t data, unsigned index, BOOL shouldDeallocate)
{
  if(index<listCount(self))
    {
      listEntry_t* thisEntry = listEntryAtIndex(self, index);
      if(shouldDeallocate)
        listCleanEntry(self, thisEntry);
      thisEntry->data = data;
    }
}

//resetIterator--------------------------------------------------------------------------
BOOL listResetIterator(List* self)
{
  self->currentEntry = self->firstEntry;
  return (self->currentEntry == NULL) ? 0 : 1;
}

//advanceIterator------------------------------------------------------------------------
BOOL listAdvanceIterator(List* self)
{
  self->currentEntry = (listEntry_t*)(self->currentEntry->next);
  return (self->currentEntry == NULL) ? 0 : 1;
}

//currentData----------------------------------------------------------------------------
listData_t listCurrentData(List* self)
{
  return self->currentEntry->data;
}

//setCurrentData-------------------------------------------------------------------------
/*
void listSetCurrentData(List* self, listData_t data, BOOL ShouldDeallocate)
{
  if(shouldDeallocate)
    listCleanEntry(self, self->currentEntry);
  self->currentEntry->data = data;
}
*/

//clear----------------------------------------------------------------------------------
void listClear(List* self, BOOL shouldDeallocate)
{
  while(listCount(self))
    listRemoveEntryAtIndex(self, 0, shouldDeallocate);
}

//defaultDataDeallocator-----------------------------------------------------------------
void listDefaultDataDeallocator(listData_t data)
{
  if(data != NULL)
    free(data);
}

//setDataDeallocator---------------------------------------------------------------------
/*
void listSetDataDeallocator (List* self, listDataDeallocator_t deallocator)
{
  self->deallocateData = deallocator;
}
*/


//---------------------------------------------------------------------------------------
void listSwapEntryContent(listEntry_t* a, listEntry_t* b)
{
  listData_t*           tempData    = a->data;
  listDataDeallocator_t tempDealloc = a->deallocateData;
  a->data           = b->data;
  a->deallocateData = b->deallocateData;
  b->data           = tempData;
  b->deallocateData = tempDealloc;
}

//---------------------------------------------------------------------------------------
unsigned listPartition(List* self, unsigned lowIndex, unsigned highIndex, listSortCallback_t sortCallback)
{
  unsigned i, result=lowIndex;
  unsigned pivotIndex = lowIndex + ((highIndex - lowIndex) / 2);
  
  listEntry_t* pivotEntry   = listEntryAtIndex(self, pivotIndex);
  listEntry_t* highEntry    = listEntryAtIndex(self, highIndex );
  listEntry_t* storeEntry   = listEntryAtIndex(self, lowIndex);
  listEntry_t* currentEntry = storeEntry;
  
  listSwapEntryContent(pivotEntry, highEntry);
  
  for(i=lowIndex; i<highIndex; i++)
    {
      if(sortCallback(highEntry->data, currentEntry->data))
        {
          listSwapEntryContent(currentEntry, storeEntry);
          storeEntry = storeEntry->next;
          result++;
        }
      currentEntry = currentEntry->next;
    }
  
  listSwapEntryContent(storeEntry, highEntry);
  
  return result;
}

//---------------------------------------------------------------------------------------
void listQuicksort(List* self, unsigned lowIndex, unsigned highIndex, listSortCallback_t sortCallback)
{
  if(lowIndex < highIndex)
    {
      unsigned p = listPartition(self, lowIndex, highIndex, sortCallback);
      
      if(p > 0) listQuicksort(self, lowIndex, p-1      , sortCallback);
      listQuicksort(self, p+1     , highIndex, sortCallback);
    }
}

//---------------------------------------------------------------------------------------
void listSort                        (List* self, listSortCallback_t sortCallback)
{
  listQuicksort(self, 0, self->count-1, sortCallback);
}

//---------------------------------------------------------------------------------------
void listSortRange                   (List* self, unsigned startIndex, unsigned range, listSortCallback_t sortCallback)
{
  if(range <= 1) return;
  
  unsigned endIndex = startIndex + range - 1;
  if(endIndex >= self->count) endIndex = self->count - 1;
  //quicksort will catch if startIndex > endIndex or startIndex > self->count
  listQuicksort(self, startIndex, endIndex, sortCallback);
}
//listDestroy----------------------------------------------------------------------------
void listDestroy(List* self, BOOL shouldDeallocate)
{
  if(self != NULL)
  {
    listClear(self, shouldDeallocate);
    free(self);
  }
}

