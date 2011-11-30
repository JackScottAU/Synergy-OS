#include <multiboot.h>
#include <vgaConsole.h>
#include <memoryManager.h>


//Holds the address of the start of the list representing free memory blocks.
memoryManager_freeMemoryNode* memoryManager_firstFreeNode = (memoryManager_freeMemoryNode*) 0xFFFFFFFF;



/**
 * 
 * @param startAddress
 * @param length
 * @param endOfReservedMemory
 */
void memoryManager_init(struct multiboot_memoryMapNode* startAddress, uint32 length, uint32 endOfReservedMemory)
{
	struct multiboot_memoryMapNode* memNode = (void*) startAddress;
	
	for(uint32 i=0; i<length/24; i++)
	{
		//Print details
		if(memNode[i].type==1)
		{
//			vgaConsole_printf("%h.%h.%h\n",(uint32)memNode[i].addr, (uint32)memNode[i].len,memNode[i].type);
			
			if(memNode[i].len>sizeof(memoryManager_freeMemoryNode))
			{
				memoryManager_freeMemoryNode* node;
				
				//Check if we're overwriting kernels or modules.
				if( (uint32)memNode[i].addr >= 0x00100000  && (uint32)memNode[i].addr <= endOfReservedMemory ) 
				{
					node = (memoryManager_freeMemoryNode*) endOfReservedMemory;
					
					node->address = (uint64) endOfReservedMemory + sizeof(memoryManager_freeMemoryNode);
					node->length = (uint64) memNode[i].len - sizeof(memoryManager_freeMemoryNode) - (endOfReservedMemory - 0x00100000);
				} else {
					node = (memoryManager_freeMemoryNode*) (uint32) memNode[i].addr; //Put a free block in the start of this area.
					
					node->address = (uint64) memNode[i].addr + sizeof(memoryManager_freeMemoryNode);
					node->length = (uint64) memNode[i].len - sizeof(memoryManager_freeMemoryNode);
				}
				
				//Do magic to try and find where to place this in the list.
				if((uint32)memoryManager_firstFreeNode==END_OF_MEMORY_LIST)
				{
					node->next = (memoryManager_freeMemoryNode*) END_OF_MEMORY_LIST;
					memoryManager_firstFreeNode = node;
				} else {
					//Add it to the front of the list. We'll figure out sorting it later.
					node->next = memoryManager_firstFreeNode;
					memoryManager_firstFreeNode = node;
				}
			} else {
				//PANIC.
			}
		}
	}
	
	
	//Display list of nodes for testing.
	memoryManager_freeMemoryNode* current = memoryManager_firstFreeNode;
	while((uint32) current != 0xFFFFFFFF)
	{
		vgaConsole_printf("In list: %h.%h\n",(uint32)current->address, (uint32)current->length);
		
		current = current->next;
	}
}