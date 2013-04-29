#ifndef BUFFER_POOL_H
#define BUFFER_POOL_H

#include <stdlib.h>
#include <queue>

template <class buffer_item> class Buffer_pool {
    private:
        buffer_item **pool = NULL;
        std::queue<int> free_buffer_index;
	int pool_size;

    public:
        Buffer_pool(int pool_size) {
	    pool = (buffer_item**)malloc(sizeof(buffer_item)*pool_size);
            for(int i=0; i<pool_size; i++){
		pool[i] = new buffer_item();
		free_buffer_index.push(i);
	    }
	}
        ~Buffer_pool() {
	    if(pool != NULL){ 
		free(pool);
		for(int i = 0; i<pool_size; i++){
		    if(pool[i] != NULL) delete pool[i];
		}
	    }
	}
        int get_buffer_id() {
	    while(free_buffer_index.empty());

	    int id = free_buffer_index.front(); 
	    free_buffer_index.pop();
	    return id; 
	}
	int get_pool_size(){
            return pool_size;
	}
        
	buffer_item* get_item(int id) {
	    return pool[id];
	}
        void release_buffer(int id) {
	    free_buffer_index.push(id);
	}
};

#endif
