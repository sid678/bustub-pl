//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"


#include <list>
#include <unordered_map>

namespace bustub {

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
  // We allocate a consecutive memory space for the buffer pool.
  pages_ = new Page[pool_size_];
  // replacer_ = new LRUReplacer(pool_size);
  replacer_ = new ClockReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete replacer_;
}

Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
  // 1.     Search the page table for the requested page (P).
  // 1.1    If P exists, pin it and return it immediately.
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  // 2.     If R is dirty, write it back to the disk.
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.

  frame_id_t frame_id;
 

  if(page_table_.find(page_id)!=page_table_.end()){

    frame_id = page_table_[page_id];
    replacer_->Pin(frame_id);

    pages_[frame_id].pin_count_++;

    return &pages_[frame_id];

  }
  else if(!free_list_.empty())
  {
    frame_id = free_list_.front();
    free_list_.pop_front();
  }
  else{

    if(replacer_->Victim(&frame_id)){

      Page *R = &pages_[frame_id]; 
      if(R->IsDirty()){
        //Write to disk;
        R->is_dirty_ = false;
        disk_manager_->WritePage(R->GetPageId(), R->GetData());
      }
      page_table_.erase(R->GetPageId());

    }
    else{

      return nullptr;
    }
  }
  
  page_table_[page_id] = frame_id;
  //change R value to P's values
  //P's values come from disk
  replacer_->Pin(frame_id);
  disk_manager_->ReadPage(page_id,pages_[frame_id].GetData());

  pages_[frame_id].page_id_ = page_id;
  pages_[frame_id].pin_count_ = 1;
  pages_[frame_id].is_dirty_ = false;

  return &pages_[frame_id];

}

bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) { 

  if(page_table_.find(page_id)==page_table_.end()){

    return false;
  }

  frame_id_t frame_id= page_table_[page_id];
  Page *P = &pages_[frame_id];
  P->pin_count_--;

  if(P->pin_count_ < 0){
    P->pin_count_ = 0;
  }

  if(P->IsDirty()){

      P->is_dirty_ = false;
      disk_manager_->WritePage(P->GetPageId(), P->GetData());
  }

  if(P->GetPinCount()==0){

    replacer_->Unpin(frame_id);  
  }

  return true; 
}

bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!
  if(page_table_.find(page_id)!=page_table_.end()){

    return false;
  }

  frame_id_t frame_id = page_table_[page_id];
  Page *P = &pages_[frame_id];
  disk_manager_->WritePage(page_id,P->GetData());
  P->is_dirty_ = false;

  return true;
}

Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {
  // 0.   Make sure you call DiskManager::AllocatePage!
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  // 4.   Set the page ID output parameter. Return a pointer to P.
  *page_id = disk_manager_->AllocatePage();
  frame_id_t frame_id;

  if(!free_list_.empty()){
    
    frame_id = free_list_.front();
    free_list_.pop_front();
    

  }
  else if(replacer_->Victim(&frame_id)){

    Page *P = &pages_[frame_id];
    
    if(P->IsDirty()){

      P->is_dirty_ = false;
      disk_manager_->WritePage(P->GetPageId(), P->GetData());

    }
    P->ResetMemory();
    
  }
  else{

    return nullptr;
  }

  page_table_[*page_id] = frame_id;
  replacer_->Pin(frame_id);
  pages_[frame_id].page_id_ = *page_id;
  pages_[frame_id].pin_count_ = 1;
  pages_[frame_id].is_dirty_ = false;

  return &pages_[frame_id];
}

bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
  // 0.   Make sure you call DiskManager::DeallocatePage!
  // 1.   Search the page table for the requested page (P).
  // 1.   If P does not exist, return true.
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.

  disk_manager_->DeallocatePage(page_id);
  frame_id_t frame_id;

  if(page_table_.find(page_id)==page_table_.end()){

    return true;
  }
  else {

    frame_id = page_table_[page_id]; 
    if(pages_[frame_id].GetPinCount()>0){

      return false;
    }
  }
  Page *P = &pages_[frame_id];
  P->is_dirty_ = false;
  P->pin_count_ = 0;
  P->page_id_ = INVALID_PAGE_ID;
  page_table_.erase(page_id);
  P->ResetMemory();
  free_list_.push_back(frame_id);

  return true;
}

void BufferPoolManager::FlushAllPagesImpl() {

  for(auto it = page_table_.begin();it != page_table_.end();++it){

    FlushPageImpl(it->first);
  }
  
  // You can do it!
}

}  // namespace bustub
