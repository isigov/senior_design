/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#define LOG_BUFFER_SIZE 1024*32

#include "srslte/common/logger_file.h"

using namespace std;

namespace srslte{

logger_file::logger_file()
  :inited(false)
  ,not_done(true)
{}

logger_file::~logger_file() {
  not_done = false;
  log(new std::string("Closing log"));
  if(inited) {
    wait_thread_finish();
    flush();
    fclose(logfile);
  }
}

void logger_file::init(std::string file) {
  pthread_mutex_init(&mutex, NULL); 
  pthread_cond_init(&not_empty, NULL);
  pthread_cond_init(&not_full, NULL);
  filename = file;
  logfile = fopen(filename.c_str(), "w");
  if(logfile==NULL) {
    printf("Error: could not create log file, no messages will be logged");
  }
  start();
  inited = true;
}

void logger_file::log(const char *msg) {
  log(new std::string(msg));
}

void logger_file::log(str_ptr msg) {
  pthread_mutex_lock(&mutex);
  buffer.push_back(msg);
  pthread_cond_signal(&not_empty);
  pthread_mutex_unlock(&mutex);
}

void logger_file::run_thread() {
  while(not_done) {
  pthread_mutex_lock(&mutex);
    while(buffer.empty()) {
      pthread_cond_wait(&not_empty, &mutex);
    }
    str_ptr s = buffer.front();
    pthread_cond_signal(&not_full);
    if(logfile)
      fprintf(logfile, "%s", s->c_str());
    delete s; 
    buffer.pop_front();
    pthread_mutex_unlock(&mutex);
  }
}

void logger_file::flush() {
  std::deque<str_ptr>::iterator it;
  for(it=buffer.begin();it!=buffer.end();it++)
  {
    str_ptr s = *it; 
    if(logfile)
      fprintf(logfile, "%s", s->c_str());
    delete s; 
  }
}

} // namespace srsue
