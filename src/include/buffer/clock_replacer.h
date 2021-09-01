//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.h
//
// Identification: src/include/buffer/clock_replacer.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <vector>
#include <unordered_map>
#include <map>
#include <iostream>

#include "buffer/replacer.h"
#include "common/config.h"




using namespace std;
namespace bustub {

/**
 * ClockReplacer implements the clock replacement policy, which approximates the Least Recently Used policy.
 */
class ClockReplacer : public Replacer {
 public:
  /**
   * Create a new ClockReplacer.
   * @param num_pages the maximum number of pages the ClockReplacer will be required to store
   */
  explicit ClockReplacer(size_t num_pages);

  /**
   * Destroys the ClockReplacer.
   */
  ~ClockReplacer() override;

  bool Victim(frame_id_t *frame_id) override;

  void Pin(frame_id_t frame_id) override;

  void Unpin(frame_id_t frame_id) override;

  size_t Size() override;

  void display_clockFrames();

 private:

  int victimize_start_flag=0;
  list<frame_id_t> :: iterator clock_hand;
  size_t mx_size;
  list<frame_id_t>clockFrames;
  unordered_map<frame_id_t,list<frame_id_t>::iterator>clockFramesLocation;
  map<frame_id_t,bool>reference_flag;
 
};

}  // namespace bustub
