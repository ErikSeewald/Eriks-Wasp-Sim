
/**
* @namespace MemoryManager
* @brief Namespace for methods handling and cleaning memory.
*/
namespace MemoryManager
{
	void startLoop();

	/**
	* Allows other functions to schedule a memory cleanup. This will not happen immediately as to avoid
	* concurrency conflicts with other threads that may still need a killed but not deleted entity
	* or something similar.
	*/
	void scheduleCleanup();

	void _memCleanup();
}