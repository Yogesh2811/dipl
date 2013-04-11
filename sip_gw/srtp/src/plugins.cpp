#include "plugins.h"
#include "utils.h"
#include <map>

#include <dirent.h>
#include <dlfcn.h>
#include <boost/algorithm/string/predicate.hpp>

#define PAYLOAD_TYPES 127 //[RFC3551]


using namespace std;
namespace ba = boost::algorithm;

static Plugins::Codec transcode_plugins[PAYLOAD_TYPES];
const char *plugin_dir = "../plugins/bin/"; 


bool is_plugin_file_name(string plugin_path){
    if(/*is_file &&*/ ba::ends_with(plugin_path,".so")){
        return true;
    }
    return false;
}

int get_PT(void* plugin){
    int *pt = (int*)dlsym(plugin, "PT");
    
    char* error;
    if((error = dlerror()) != NULL)
        return -1;
    
    return *pt;
}

void* get_plugin(string plugin_name){
    string tmp_dir(plugin_dir);
    void *plugin = dlopen(tmp_dir.append(plugin_name).c_str(), RTLD_LAZY);
    return plugin;
}

int set_plugin(void* plugin){
    char* error;

    int pt = get_PT(plugin);
    if(pt < 0) {return 1;}

    char** encoding_name = (char**)dlsym(plugin, "encoding_name");
    if((error = dlerror()) != NULL) {return 2;}

    int (**transcode)(CBYTE*, BYTE*, int, int*, int);
    transcode = (int(**)(CBYTE*,BYTE*,int,int*,int))dlsym(plugin, "transcode");
    if((error = dlerror()) != NULL) {return 3;}

    void (**to_raw)(CBYTE*, BYTE*, int, int*);
    to_raw = (void(**)(CBYTE*,BYTE*,int,int*))dlsym(plugin, "to_raw");
    if((error = dlerror()) != NULL) {return 4;}
 
    void (**from_raw)(CBYTE*, BYTE*, int, int*);
    from_raw = (void(**)(CBYTE*,BYTE*,int,int*))dlsym(plugin, "from_raw");
    if((error = dlerror()) != NULL) {return 5;}

    transcode_plugins[pt].PT = pt; 
    transcode_plugins[pt].encoding_name = (*encoding_name);
    transcode_plugins[pt].transcode = (*transcode);
    transcode_plugins[pt].to_raw = (*to_raw);
    transcode_plugins[pt].from_raw = (*from_raw);

    LOG_MSG("Plugin::set_plugin() PT:%d name:%s", pt, (*encoding_name));
    return 0;
}

    
void Plugins::init(){
    DIR *dir;
    struct dirent *plugin_file;
    void *plugin;

    if((dir = opendir (plugin_dir)) != NULL){
        LOG_MSG("Plugins::init()");
        
        while ((plugin_file = readdir (dir)) != NULL) {
            if(is_plugin_file_name(plugin_file->d_name)){
                LOG_MSG("Plugins::init() - loading %s", plugin_file->d_name);
                plugin = get_plugin(plugin_file->d_name);
                if(plugin != NULL) {
                    int err = set_plugin(plugin);
                    if(err != 0){ 
                        LOG_ERROR("Plugins::init()%s %d", dlerror(), err) break;
                    }
                    int pt = get_PT(plugin);
                    (*transcode_plugins[pt].transcode)(NULL, NULL, 0, NULL, 0);
                    (*transcode_plugins[pt].to_raw)(NULL, NULL, 0, NULL);    
                    (*transcode_plugins[pt].from_raw)(NULL, NULL, 0, NULL);
                }
            }
        }
        
        closedir(dir);
        /*for (; itr != end_itr; itr++ ) {
            if ( is_plugin_file_name(itr->path().string())) {
                //LOG_MSG("Plugins::init() %s loaded",itr->path().string().c_str());
                //transcode_plugins[ get_PT(itr->path()) ] = get_plugin(itr->path());
            }
        }*/

    } else {
        LOG_ERROR("Plugins::init() does not exist");
    }
}


void Plugins::cleanup(){
    for(int i = 0; i<PAYLOAD_TYPES; i++){
        //if(transcode_plugins[i] != NULL){
            //free(transcode_plugins[i]);
        //}
    }
}
