/** @file */
#ifndef BUFFER_POOL_H
#define BUFFER_POOL_H

#include <stdlib.h>
#include <queue>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

/** \brief Semaphore implementation for synchronization*/
class Semaphore {
private:
    boost::mutex mutex_;
    boost::condition_variable condition_;
    unsigned long count_;

public:
    semaphore()
        : count_()
    {}
    /** \brief notification of waiters*/
    void notify(){
        boost::mutex::scoped_lock lock(mutex_);
        ++count_;
        condition_.notify_one();
    }
    /** \brief waiting for the release*/
    void wait(){
        boost::mutex::scoped_lock lock(mutex_);
        while(!count_)
            condition_.wait(lock);
        --count_;
    }
};

/** \brief Implementation for thread safe buffer pools*/
template <class buffer_item> class Buffer_pool {
    private:
        buffer_item **pool = NULL;
        std::queue<int> free_buffer_index;
	      int pool_size;

    public:
        /** \brief initialization of pool structures
         *        
         * @param pool_size number of the buffers in pool       
         */
        Buffer_pool(int pool_size) {
            pool = (buffer_item**)malloc(sizeof(buffer_item*)*pool_size);
            for(int i=0; i<pool_size; i++){
		            pool[i] = new buffer_item();
		            free_buffer_index.push(i);
	          }
	      }
        /** \brief dealocation of resources*/
        ~Buffer_pool() {
	          if(pool != NULL){ 
		            for(int i = 0; i<pool_size; i++){
		                if(pool[i] != NULL) delete pool[i];
		            }
		            free(pool);
	          }
	      }
        /** \brief allocates and returns unique buffer id*/
        int get_buffer_id() {
	          while(free_buffer_index.empty());

	          int id = free_buffer_index.front(); 
	          free_buffer_index.pop();
	          return id; 
	      }
        /** \brief returns number of free buffers in the pool*/
	      int get_pool_size(){
            return free_buffer_index.size();
	      }
        /** \brief returns buffer item with specified index
         *
         * @id index to the buffer pool
         */                          
	      buffer_item* get_item(int id) {
	          return pool[id];
	      }
        /** \brief releases the buffer with specified index
         *        
         * @param id index to the buffer pool       
         */
        void release_buffer(int id) {
	          free_buffer_index.push(id);
	      }
};

#endif
/** @} */