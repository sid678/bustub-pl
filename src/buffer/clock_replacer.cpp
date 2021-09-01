//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.cpp
//
// Identification: src/buffer/clock_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/clock_replacer.h"


namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) {

    mx_size = num_pages;
}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) { 


    return false; 
    
}

void ClockReplacer::Pin(frame_id_t frame_id) {

    auto frame_id_location = clockFramesLocation[frame_id];
    clockFrames.erase(frame_id_location);
    reference_flag.erase(frame_id);

}

void ClockReplacer::Unpin(frame_id_t frame_id) {

    clockFrames.push_back(frame_id);
    reference_flag[frame_id] = 0;
    auto frame_id_location = clockFrames.end();
    frame_id_location--;
    clockFramesLocation[frame_id] =  frame_id_location;

}

size_t ClockReplacer::Size() {
    
     return clockFrames.size(); 
}

void ClockReplacer :: display_clockFrames(){

    for(auto it:clockFrames)
    {
        cout<<it<<" ";
    }
    cout<<"\n";

}

}  // namespace bustub
