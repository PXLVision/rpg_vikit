/*
 * user_input_thread.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: pizzoli, cforster
 */

#include <stdio.h>
#include <vikit/user_input_thread.h>

namespace vk {

UserInputThread::UserInputThread() :
    stop_(false),
    input_( (char) 0)
{
  tcgetattr(0, &original_terminal_settings_); // save old terminal i/o settings
  new_terminal_settings_ = original_terminal_settings_; // make new settings same as old settings
  new_terminal_settings_.c_lflag &= ~ICANON; // disable buffered i/o
  new_terminal_settings_.c_lflag &= ~ECHO; // set echo mode
  new_terminal_settings_.c_cc[VMIN] = 1; //minimum of number input read.
  tcsetattr(0, TCSANOW, &new_terminal_settings_); // use these new terminal i/o settings now

  user_input_thread_ = new boost::thread(&UserInputThread::acquireUserInput, this);
}

UserInputThread::~UserInputThread()
{
  tcsetattr(0, TCSANOW, &original_terminal_settings_);
  user_input_thread_->interrupt();
  user_input_thread_->join();
}

char UserInputThread::getInput()
{
  char tmp = input_;
  input_ = (char) 0;
  return tmp;
}

void UserInputThread::stop()
{
  stop_ = true;
}

void UserInputThread::acquireUserInput()
{
  int c = 0;
  while(!stop_)
  {
    c = getchar();  // TODO: this is blocking, so the interruption point is not reached...
    if ((char)c == ' ')
      printf("USER INPUT: SPACE\n");
    else
      printf("USER INPUT: %c\n", (char) c);
    input_ = (char) c;
    c = 0;

    // interruption point:
    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
  }
}

} // end namespace vk


