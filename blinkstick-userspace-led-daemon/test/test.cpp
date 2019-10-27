// MIT License
//
// Copyright (c) 2019 Keith T. Garner
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include <iostream>

#include <blinkstick_userspace_led_daemon/LEDBinding.hpp>
#include <blinkstick_userspace_led_daemon/LEDBindingRegistrationException.hpp>
#include <blinkstick_userspace_led_daemon/BlinkStick.hpp>

using namespace BlinkstickUserspace;

int main()
{
  hid_init();

  BlinkStickPtr blinkstick = BlinkStick::find();

  LEDBindingVector ledBindings;
  for (int i = 0; i < 8; i++)
  {
    std::string ledName("blinkstick::");
    ledName.append(std::to_string(i));
    try
    {
      LEDBindingPtr ledBinding = LEDBindingPtr(new LEDBinding(ledName, blinkstick, i));
      ledBindings.push_back(ledBinding);
    }
    catch (LEDBindingRegistrationException &e)
    {
      std::cerr << e.getMessage() << ", errno: " << e.getErrorNo() << std::endl;
    }
  }

  const size_t bindingsCount = ledBindings.size();
  struct pollfd fds[bindingsCount];

  for (unsigned int i = 0; i < bindingsCount; i++)
  {
    fds[i] = ledBindings[i]->getPollFd();
  }

  while (1)
  {
    int events = poll(fds, bindingsCount, -1);

    if (events > 0)
    {
      for (unsigned int i = 0; i < bindingsCount; i++)
      {
        if (fds[i].revents && POLLIN)
        {
          ledBindings[i]->processBrightnessChange();
        }
      }
    }
  }
}
