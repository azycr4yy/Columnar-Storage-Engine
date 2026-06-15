#ifndef buffer.h
#define buffer.h

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <stdint.h>
#include <chrono>
#include <stdexcept>
#include <print>

static constexpr size_t PAGE_SIZE = 4096;

struct Page{
    uint8_t data[PAGE_SIZE];
    int page_id;
};

struct Frame{
    Page* page; //storing the page id and its data
    int pin_count; //number of users using this page
    bool is_dirty; // dirty page - > modified
    uint64_t last_used; //tracker for LRU algo
    Frame():page(nullptr),pin_count(0),is_dirty(false),last_used(INT64_MAX){}
};

class BufferManager{
    private:
    uint64_t m_getTime(){
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    public:
    int capacity;
    std::vector<Frame> frames;
    std::unordered_map<int,int> page_table;
    BufferManager(int capacity){
        this->capacity = capacity;
        frames.resize(capacity);
    }
    void unpin(int page_id){
        if(page_table.count(page_id) == 0){
            throw std::runtime_error("Page not in buffer pool");
        }
        int frame_idx = page_table[page_id];
        if(frames[frame_idx].pin_count > 0){
        frames[frame_idx].pin_count--;
        }     
    }
    void flush(int page_id){
        if(page_table.count(page_id) == 0){
            throw std::runtime_error("Page not in buffer pool");
        }
        int frame_idx = page_table[page_id];
        if(frames[frame_idx].is_dirty){
            //save to disk before flushing
        }
        frames[frame_idx].is_dirty = false;
        }
    int find_free_frame(){
        for(int i= 0;i<capacity;i++){
            if(frames[i].page==nullptr){
                return i;
            }
        }
        return -1;
    }
    int evict_LRU(){
        int lru_index = -1;
        uint64_t max_lru_time = INT64_MAX;
        for(int i=0;i<capacity;i++){
            if(frames[i].pin_count==0 && frames[i].last_used<max_lru_time){
                lru_index = i;
                max_lru_time = frames[i].last_used;
            }
        }
        if (lru_index == -1){
            throw std::runtime_error("No free frames");
        }
        page_table.erase(frames[lru_index].page->page_id);
        delete frames[lru_index].page;
        frames[lru_index].page = nullptr;
        return lru_index;
    }
    Page* fetch(int page_id){
        if(page_table.count(page_id) > 0){
            int frame_idx = page_table[page_id];
            frames[frame_idx].last_used = m_getTime();
            frames[frame_idx].pin_count++;
            return frames[frame_idx].page;
        }
        int frame_idx = find_free_frame();
        if(frame_idx == -1){
            frame_idx = evict_LRU();
        }
        page_table[page_id] = frame_idx;
        frames[frame_idx].page = new Page();
        frames[frame_idx].page->page_id = page_id;
        frames[frame_idx].pin_count = 1;
        frames[frame_idx].last_used = m_getTime();
        frames[frame_idx].is_dirty = false;
        return frames[frame_idx].page;

    }
};

#endif