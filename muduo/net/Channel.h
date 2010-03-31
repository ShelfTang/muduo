// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_CHANNEL_H
#define MUDUO_NET_CHANNEL_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <muduo/base/Timestamp.h>

namespace muduo
{
namespace net
{

class EventLoop;

///
/// A selectable I/O channel.
///
/// This class doesn't own the file descriptor.
/// The file descriptor could be a socket,
/// an eventfd, a timerfd, or a signalfd
class Channel : boost::noncopyable
{
 public:
  typedef boost::function<void()> EventCallback;
  typedef boost::function<void(Timestamp)> ReadEventCallback;
  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;
  // static const int kErrorvent;

  Channel(EventLoop* loop, int fd);
  ~Channel();

  void handleEvent(Timestamp receiveTime);
  void setReadCallback(const ReadEventCallback& cb)
  { readCallback_ = cb; }
  void setWriteCallback(const EventCallback& cb)
  { writeCallback_ = cb; }
  void setCloseCallback(const EventCallback& cb)
  { closeCallback_ = cb; }
  void setErrorCallback(const EventCallback& cb)
  { errorCallback_ = cb; }

  /// Tie this channel to the owner object managed by shared_ptr,
  /// prevent the owner object being destroyed in handleEvent.
  void tie(const boost::shared_ptr<void>&);

  int fd() { return fd_; }
  int events() { return events_; }
  void set_events(int evt) { events_ = evt; }
  void set_revents(int revt) { revents_ = revt; }

  int index() { return index_; }
  void set_index(int idx) { index_ = idx; }

  EventLoop* getLoop() { return loop_; }

  // void set_loop(EventLoop* loop) { loop_ = loop; }

 private:
  void handleEventWithGuard(Timestamp receiveTime);

  EventLoop* loop_;
  const int  fd_;
  int        events_;
  int        revents_;
  int        index_; // used by PollPoller.

  boost::weak_ptr<void> tie_;
  bool tied_;
  ReadEventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback closeCallback_;
  EventCallback errorCallback_;
};

}
}
#endif  // MUDUO_NET_CHANNEL_H
