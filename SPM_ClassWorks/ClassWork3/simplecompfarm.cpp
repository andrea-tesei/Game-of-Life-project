/*
 * Naive file compressor using miniz, a single C source file Deflate/Inflate compression 
 * library with zlib-compatible API (Project page: https://code.google.com/p/miniz/).
 *
 * To compile:
 *   g++ -std=c++11 -I$FF_ROOT -O3 simplecompressor.cpp -o simplecompressor
 */

#include "miniz.c"
#include <sys/mman.h>
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

using namespace ff;

typedef struct resultType{
    std::string filename;
    unsigned char *file;
    size_t length;
    unsigned long cmp_len;
} resultType_t;

// map the file pointed by filepath in memory
static inline bool mapFile(const std::string &filepath, unsigned char *&ptr, size_t &size) {
    // open input file.
    int fd = open(filepath.c_str(),O_RDONLY);
    if (fd<0) {
        printf("Failed opening file %s\n", filepath.c_str());
        return false;
    }
    struct stat s;
    if (fstat (fd, &s)) {
        printf("Failed to stat file %s\n", filepath.c_str());
        return false;
    }
    // checking for regular file type
    if (!S_ISREG(s.st_mode)) return false;
    // get the size of the file
    size = s.st_size;
    // map all the file in memory
    ptr = (unsigned char *) mmap (0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        printf("Failed to map file %s in memory\n", filepath.c_str());
        return false;
    }
    close(fd);
    return true;
}
// unmap the file from memory
static inline void unmapFile(unsigned char *ptr, size_t size) {
    if (munmap(ptr, size)<0) {
	printf("Failed to unmap file\n");
    }
}
// write size bytes starting from ptr into a file pointed by filename
static inline void writeFile(const std::string &filename, unsigned char *ptr, size_t size) {
    FILE *pOutfile = fopen(filename.c_str(), "wb");
    if (!pOutfile) {
        printf("Failed opening output file %s!\n", filename.c_str());
        return;
    }
    if (fwrite(ptr, 1, size, pOutfile) != size) {
        printf("Failed writing to output file %s\n", filename.c_str());
        return;
    }
    fclose(pOutfile);
}

struct firstStage: ff_node_t<resultType_t> {
    std::vector<std::string>& inputFiles;
    int lengthOfStream;
    
    
    firstStage(std::vector<std::string>& input, int length) : inputFiles(input){
        lengthOfStream = length;
    }
    
    resultType_t *svc(resultType_t *input) {
        while(!inputFiles.empty()){
            resultType_t *t = new resultType_t();
            t->filename = inputFiles.back();
            inputFiles.pop_back();
            t->file = nullptr;
            t->length = 0;
            ff_send_out(t);
        }
        return EOS; // End-Of-Stream
    }
};

struct CompressStage: ff_node_t<resultType_t>{
    resultType_t *svc(resultType_t *task){
        const std::string &filepath(task->filename);
        
        unsigned char * ptr = nullptr;
        size_t size = 0;
        
        if (!mapFile(filepath, ptr, size))
            std::cout << "Failed to map the file in memory" << std::endl;
        task->length = size;
        // get an estimation of the maximum compression size
        unsigned long cmp_len = compressBound(size);
        // allocate memory to store compressed data in memory
        task->file = new unsigned char[cmp_len];
        task->cmp_len = cmp_len;
        std::cout << "Compressing file... \n";
        if (compress(task->file, &cmp_len, (const unsigned char *)ptr, size) != Z_OK) {
            printf("Failed to compress file in memory\n");
        }
        unmapFile(ptr, size);
        return task;
    }
    
};

struct WriteToDisk: ff_node_t<resultType>{
    resultType *svc(resultType* task){
        std::string filename;
        std::cout << "Writing to disk..." << std::endl;
        int n=task->filename.find_last_of("/");
        if (n>0)
            filename = task->filename.substr(n+1);
        else
            filename = task->filename;
        const std::string outfile = "./out/" + filename + ".zip";
        std::cout << "Compressed file " << filename << " from " << task->cmp_len << " to " << task->length << "\n";
        // write the compressed data into disk
        writeFile(outfile, task->file, task->length);
        delete task;
        return GO_ON;
    }
};


int main(int argc, char *argv[]){
    if (argc < 2) {
        std::cerr << "use: " << argv[0] << " file [file]\n";
        return -1;
    }
    std::vector<std::string> input;
    std::vector<std::unique_ptr<ff_node>> workers;
    firstStage n1(input, argc);
    //CompressStage n2;
    WriteToDisk n3;
    for(int i = 0; i < 3; i++)
        workers.push_back(make_unique<CompressStage>());
    for(int i = 1; i < argc; i++)
        input.push_back(std::string(argv[i]));
    // TO-DO: farm intelligente che smista i file in base al carico
    ff_Farm<resultType_t> farmz(std::move(workers));
    ff_Pipe<resultType_t> pipe(n1,farmz,n3);
    int result = pipe.run_and_wait_end();
    if (result<0) error("running pipe");
    //std::cout << "The final result is: " << n3.sum << "\n";
    return 0;
}
