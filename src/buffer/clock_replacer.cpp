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


    if(Size()==0){

        victimize_start_flag =0; 
        return false;
    }
        

    // list <frame_id_t> :: iterator frame_id_location ;

    if(victimize_start_flag==0){

        clock_hand = clockFrames.begin();   
        victimize_start_flag = 1;

    }

    while(reference_flag[*clock_hand]==1){
                
        reference_flag[*clock_hand]=0;
        clock_hand++;

        if(clock_hand == clockFrames.end()){
            
            clock_hand = clockFrames.begin();
        }
    }
    // cout<<"Crossed while loop\n";
    *frame_id = *clock_hand;
    // cout<<*frame_id<<endl;
    auto temp = clock_hand;
    clock_hand++;
    clockFrames.erase(temp);
    clockFramesLocation.erase(*frame_id);
    reference_flag.erase(*frame_id);

    return true;   
    
}

void ClockReplacer::Pin(frame_id_t frame_id) {

    // cout<<"Pin\n";

    if(clockFramesLocation.find(frame_id)!=clockFramesLocation.end()){

        
        auto frame_id_location = clockFramesLocation[frame_id];

        if(clock_hand==frame_id_location){
            clock_hand++;
        }

        clockFrames.erase(frame_id_location);
        reference_flag.erase(frame_id);
        clockFramesLocation.erase(frame_id);
    }
    

}

void ClockReplacer::Unpin(frame_id_t frame_id) {

    // cout<<"Unpin\n";
    if(clockFramesLocation.find(frame_id)==clockFramesLocation.end()){

        clockFrames.push_back(frame_id);
        reference_flag[frame_id] = 1;
        auto frame_id_location = clockFrames.end();
        frame_id_location--;
        clockFramesLocation[frame_id] =  frame_id_location;

    }
    else
    {
        reference_flag[frame_id] = 1;
    }
    
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
