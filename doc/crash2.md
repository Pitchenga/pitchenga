-------------------------------------
Translated Report (Full Report Below)
-------------------------------------
Process:             Pitchenga [93003]
Path:                /Users/USER/*/Pitchenga.app/Contents/MacOS/Pitchenga
Identifier:          com.yourcompany.Pitchenga
Version:             1.0.0 (1.0.0)
Code Type:           ARM-64 (Native)
Role:                Foreground
Parent Process:      clion [70060]
Coalition:           com.jetbrains.CLion [56339]
Responsible Process: clion [70060]
User ID:             501

Date/Time:           2026-04-25 18:50:54.5379 +0200
Launch Time:         2026-04-25 18:49:55.6387 +0200
Hardware Model:      Mac16,11
OS Version:          macOS 26.3.1 (25D2128)
Release Type:        User

Crash Reporter Key:  31EF1460-9124-D2FD-4CAC-4A806E474698
Incident Identifier: E45405C0-E2F2-4F2A-9100-AB7DB2DCC634

Sleep/Wake UUID:       D4168D9B-1C8D-4B3E-BF3B-BB54A935B8C4

Time Awake Since Boot: 190000 seconds
Time Since Wake:       32894 seconds

System Integrity Protection: enabled

Triggered by Thread: 0  JUCE v8.0.12: Message Thread, Dispatch Queue: com.apple.main-thread

Exception Type:    EXC_BAD_ACCESS (SIGSEGV)
Exception Subtype: KERN_INVALID_ADDRESS at 0x140f1f0ef9400010
Exception Codes:   0x0000000000000001, 0x140f1f0ef9400010

Termination Reason:  Namespace SIGNAL, Code 11, Segmentation fault: 11
Terminating Process: exc handler [93003]


VM Region Info: 0x1f0ef9400010 is not in any region.  
      REGION TYPE                    START - END         [ VSIZE] PRT/MAX SHRMOD  REGION DETAIL
      UNUSED SPACE AT START
--->  
      UNUSED SPACE AT END

Thread 0 Crashed:: JUCE v8.0.12: Message Thread Dispatch queue: com.apple.main-thread
0   Splice INSTRUMENT             	       0x111062500 0x110a80000 + 6169856
1   Splice INSTRUMENT             	       0x111289944 0x110a80000 + 8427844
2   Splice INSTRUMENT             	       0x1112898bc 0x110a80000 + 8427708
3   CoreFoundation                	       0x182f86a08 __CFRUNLOOP_IS_CALLING_OUT_TO_A_SOURCE0_PERFORM_FUNCTION__ + 28
4   CoreFoundation                	       0x182f8699c __CFRunLoopDoSource0 + 172
5   CoreFoundation                	       0x182f8676c __CFRunLoopDoSources0 + 332
6   CoreFoundation                	       0x182f85398 __CFRunLoopRun + 820
7   CoreFoundation                	       0x18303ff3c _CFRunLoopRunSpecificWithOptions + 532
8   HIToolbox                     	       0x18faf0790 RunCurrentEventLoopInMode + 316
9   HIToolbox                     	       0x18faf3ab8 ReceiveNextEventCommon + 488
10  HIToolbox                     	       0x18fc7db64 _BlockUntilNextEventMatchingListInMode + 48
11  AppKit                        	       0x1878a2014 _DPSBlockUntilNextEventMatchingListInMode + 236
12  AppKit                        	       0x18739fe48 _DPSNextEvent + 588
13  AppKit                        	       0x187e66f08 -[NSApplication(NSEventRouting) _nextEventMatchingEventMask:untilDate:inMode:dequeue:] + 688
14  AppKit                        	       0x187e66c14 -[NSApplication(NSEventRouting) nextEventMatchingMask:untilDate:inMode:dequeue:] + 72
15  AppKit                        	       0x187398780 -[NSApplication run] + 368
16  Pitchenga                     	       0x1049512dc juce::JUCEApplicationBase::main() + 192
17  Pitchenga                     	       0x1049511cc juce::JUCEApplicationBase::main(int, char const**) + 88
18  dyld                          	       0x182b1dd54 start + 7184

Thread 1:

Thread 2:: com.apple.NSEventThread
0   libsystem_kernel.dylib        	       0x182ea4c34 mach_msg2_trap + 8
1   libsystem_kernel.dylib        	       0x182eb7028 mach_msg2_internal + 76
2   libsystem_kernel.dylib        	       0x182ead98c mach_msg_overwrite + 484
3   libsystem_kernel.dylib        	       0x182ea4fb4 mach_msg + 24
4   CoreFoundation                	       0x182f86bb0 __CFRunLoopServiceMachPort + 160
5   CoreFoundation                	       0x182f85508 __CFRunLoopRun + 1188
6   CoreFoundation                	       0x18303ff3c _CFRunLoopRunSpecificWithOptions + 532
7   AppKit                        	       0x18742fa34 _NSEventThread + 184
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 3:
0   libsystem_kernel.dylib        	       0x182ea4c34 mach_msg2_trap + 8
1   libsystem_kernel.dylib        	       0x182eb7028 mach_msg2_internal + 76
2   libsystem_kernel.dylib        	       0x182ead98c mach_msg_overwrite + 484
3   libsystem_kernel.dylib        	       0x182ea4fb4 mach_msg + 24
4   CoreMIDI                      	       0x1a1e4ef28 XServerMachPort::ReceiveMessage(int&, void*, int&) + 104
5   CoreMIDI                      	       0x1a1e7123c MIDIProcess::MIDIInPortThread::Run() + 148
6   CoreMIDI                      	       0x1a1e652e8 CADeprecated::XThread::RunHelper(void*) + 48
7   CoreMIDI                      	       0x1a1e709c0 CADeprecated::CAPThread::Entry(void*) + 96
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 4:: caulk.messenger.shared:17
0   libsystem_kernel.dylib        	       0x182ea4bb0 semaphore_wait_trap + 8
1   caulk                         	       0x18f578e08 caulk::semaphore::timed_wait(double) + 224
2   caulk                         	       0x18f578cb0 caulk::concurrent::details::worker_thread::run() + 32
3   caulk                         	       0x18f578950 void* caulk::thread_proxy<std::__1::tuple<caulk::thread::attributes, void (caulk::concurrent::details::worker_thread::*)(), std::__1::tuple<caulk::concurrent::details::worker_thread*>>>(void*) + 96
4   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
5   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 5:: caulk.messenger.shared:high
0   libsystem_kernel.dylib        	       0x182ea4bb0 semaphore_wait_trap + 8
1   caulk                         	       0x18f578e08 caulk::semaphore::timed_wait(double) + 224
2   caulk                         	       0x18f578cb0 caulk::concurrent::details::worker_thread::run() + 32
3   caulk                         	       0x18f578950 void* caulk::thread_proxy<std::__1::tuple<caulk::thread::attributes, void (caulk::concurrent::details::worker_thread::*)(), std::__1::tuple<caulk::concurrent::details::worker_thread*>>>(void*) + 96
4   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
5   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 6:: com.apple.audio.IOThread.client
0   libsystem_kernel.dylib        	       0x182ea4bbc semaphore_wait_signal_trap + 8
1   caulk                         	       0x18f595fac caulk::mach::semaphore::wait_signal_or_error(caulk::mach::semaphore&) + 36
2   CoreAudio                     	       0x1862016cc HALC_ProxyIOContext::IOWorkLoop() + 5052
3   CoreAudio                     	       0x1861ffc68 invocation function for block in HALC_ProxyIOContext::HALC_ProxyIOContext(unsigned int, unsigned int) + 172
4   CoreAudio                     	       0x1863c4f0c HALC_IOThread::Entry(void*) + 88
5   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
6   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 7:: com.apple.audio.IOThread.client
0   libsystem_kernel.dylib        	       0x182ea4bbc semaphore_wait_signal_trap + 8
1   caulk                         	       0x18f595fac caulk::mach::semaphore::wait_signal_or_error(caulk::mach::semaphore&) + 36
2   CoreAudio                     	       0x1862016cc HALC_ProxyIOContext::IOWorkLoop() + 5052
3   CoreAudio                     	       0x1861ffc68 invocation function for block in HALC_ProxyIOContext::HALC_ProxyIOContext(unsigned int, unsigned int) + 172
4   CoreAudio                     	       0x1863c4f0c HALC_IOThread::Entry(void*) + 88
5   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
6   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 8:: LockFreeLogger
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x110ef3ad8 0x110a80000 + 4668120
4   Splice INSTRUMENT             	       0x110ee0e00 0x110a80000 + 4591104
5   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
6   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
7   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
8   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 9:: JUCE v8.0.8: Timer
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112875b4 0x110a80000 + 8418740
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 10:: Work Thread 0
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x110ef3ad8 0x110a80000 + 4668120
4   Splice INSTRUMENT             	       0x110e78484 0x110a80000 + 4162692
5   Splice INSTRUMENT             	       0x110ee0e00 0x110a80000 + 4591104
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 11:: Work Thread 1
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e18754 std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&) + 32
3   Splice INSTRUMENT             	       0x110ef3ab4 0x110a80000 + 4668084
4   Splice INSTRUMENT             	       0x110ee0e00 0x110a80000 + 4591104
5   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
6   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
7   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
8   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 12:: Work Thread 2
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e18754 std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&) + 32
3   Splice INSTRUMENT             	       0x110ef3ab4 0x110a80000 + 4668084
4   Splice INSTRUMENT             	       0x110ee0e00 0x110a80000 + 4591104
5   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
6   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
7   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
8   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 13:: Work Thread 3
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e18754 std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&) + 32
3   Splice INSTRUMENT             	       0x110ef3ab4 0x110a80000 + 4668084
4   Splice INSTRUMENT             	       0x110ee0e00 0x110a80000 + 4591104
5   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
6   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
7   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
8   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 14:: Work Thread 4
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e18754 std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&) + 32
3   Splice INSTRUMENT             	       0x110ef3ab4 0x110a80000 + 4668084
4   Splice INSTRUMENT             	       0x110ee0e00 0x110a80000 + 4591104
5   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
6   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
7   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
8   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 15:: Work Thread 5
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e18754 std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&) + 32
3   Splice INSTRUMENT             	       0x110ef3ab4 0x110a80000 + 4668084
4   Splice INSTRUMENT             	       0x110ba5184 0x110a80000 + 1200516
5   Splice INSTRUMENT             	       0x110ee0e00 0x110a80000 + 4591104
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 16:: Work Thread 6
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e18754 std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&) + 32
3   Splice INSTRUMENT             	       0x110ef3ab4 0x110a80000 + 4668084
4   Splice INSTRUMENT             	       0x110ba5184 0x110a80000 + 1200516
5   Splice INSTRUMENT             	       0x110ee0e00 0x110a80000 + 4591104
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 17:: WaveFS
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e18754 std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&) + 32
3   Splice INSTRUMENT             	       0x110ef3ab4 0x110a80000 + 4668084
4   Splice INSTRUMENT             	       0x110ee0e00 0x110a80000 + 4591104
5   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
6   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
7   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
8   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 18:: Controller
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x110ef3ad8 0x110a80000 + 4668120
4   Splice INSTRUMENT             	       0x110b983f4 0x110a80000 + 1147892
5   Splice INSTRUMENT             	       0x110ee0e00 0x110a80000 + 4591104
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 19:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 20:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 21:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 22:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 23:: Pool
0   libsystem_kernel.dylib        	       0x182ead6c0 poll + 8
1   Splice INSTRUMENT             	       0x1116635d4 0x110a80000 + 12465620
2   Splice INSTRUMENT             	       0x111657e78 0x110a80000 + 12418680
3   Splice INSTRUMENT             	       0x111631480 0x110a80000 + 12260480
4   Splice INSTRUMENT             	       0x111603f58 0x110a80000 + 12074840
5   Splice INSTRUMENT             	       0x1115f3048 0x110a80000 + 12005448
6   Splice INSTRUMENT             	       0x1115f1900 0x110a80000 + 11999488
7   Splice INSTRUMENT             	       0x1115f07dc 0x110a80000 + 11995100
8   Splice INSTRUMENT             	       0x1115f59c0 0x110a80000 + 12016064
9   Splice INSTRUMENT             	       0x1115f5ce0 0x110a80000 + 12016864
10  Splice INSTRUMENT             	       0x1112b7770 0x110a80000 + 8615792
11  Splice INSTRUMENT             	       0x1112de1f0 0x110a80000 + 8774128
12  Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
13  Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
14  libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
15  libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 24:: Pool
0   libsystem_kernel.dylib        	       0x182ead6c0 poll + 8
1   Splice INSTRUMENT             	       0x1116635d4 0x110a80000 + 12465620
2   Splice INSTRUMENT             	       0x111657e78 0x110a80000 + 12418680
3   Splice INSTRUMENT             	       0x111631480 0x110a80000 + 12260480
4   Splice INSTRUMENT             	       0x111603f58 0x110a80000 + 12074840
5   Splice INSTRUMENT             	       0x1115f3048 0x110a80000 + 12005448
6   Splice INSTRUMENT             	       0x1115f1900 0x110a80000 + 11999488
7   Splice INSTRUMENT             	       0x1115f07dc 0x110a80000 + 11995100
8   Splice INSTRUMENT             	       0x1115f59c0 0x110a80000 + 12016064
9   Splice INSTRUMENT             	       0x1115f5ce0 0x110a80000 + 12016864
10  Splice INSTRUMENT             	       0x1112b7770 0x110a80000 + 8615792
11  Splice INSTRUMENT             	       0x1112de1f0 0x110a80000 + 8774128
12  Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
13  Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
14  libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
15  libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 25:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 26:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 27:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 28:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 29:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 30:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 31:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 32:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 33:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 34:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 35:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 36:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 37:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 38:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 39:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 40:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 41:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 42:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 43:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 44:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 45:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 46:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 47:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 48:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 49:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 50:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 51:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 52:: JUCE v8.0.12: Timer
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Pitchenga                     	       0x104a141ec std::__1::cv_status std::__1::condition_variable::wait_until[abi:nqe210106]<std::__1::chrono::steady_clock, std::__1::chrono::duration<double, std::__1::ratio<1l, 1000000000l>>>(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::steady_clock, std::__1::chrono::duration<double, std::__1::ratio<1l, 1000000000l>>> const&) + 276
4   Pitchenga                     	       0x1049b0a10 juce::WaitableEvent::wait(double) const + 236
5   Pitchenga                     	       0x104959dec juce::Timer::TimerThread::run() + 460
6   Pitchenga                     	       0x1049b14e4 juce::Thread::threadEntryPoint() + 292
7   Pitchenga                     	       0x104a09a74 juce::Thread::createNativeThread(juce::Thread::Priority)::$_0::__invoke(void*) + 156
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 53:: VisualizeWorker
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Pitchenga                     	       0x104a141ec std::__1::cv_status std::__1::condition_variable::wait_until[abi:nqe210106]<std::__1::chrono::steady_clock, std::__1::chrono::duration<double, std::__1::ratio<1l, 1000000000l>>>(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::steady_clock, std::__1::chrono::duration<double, std::__1::ratio<1l, 1000000000l>>> const&) + 276
4   Pitchenga                     	       0x1049b0a10 juce::WaitableEvent::wait(double) const + 236
5   Pitchenga                     	       0x104620fd8 Math::run() + 440
6   Pitchenga                     	       0x1049b14e4 juce::Thread::threadEntryPoint() + 292
7   Pitchenga                     	       0x104a09a74 juce::Thread::createNativeThread(juce::Thread::Priority)::$_0::__invoke(void*) + 156
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 54:

Thread 55:

Thread 56:

Thread 57:: WebCore: Scrolling
0   libsystem_kernel.dylib        	       0x182ea4c34 mach_msg2_trap + 8
1   libsystem_kernel.dylib        	       0x182eb7028 mach_msg2_internal + 76
2   libsystem_kernel.dylib        	       0x182ead98c mach_msg_overwrite + 484
3   libsystem_kernel.dylib        	       0x182ea4fb4 mach_msg + 24
4   CoreFoundation                	       0x182f86bb0 __CFRunLoopServiceMachPort + 160
5   CoreFoundation                	       0x182f85508 __CFRunLoopRun + 1188
6   CoreFoundation                	       0x18303ff3c _CFRunLoopRunSpecificWithOptions + 532
7   CoreFoundation                	       0x182fd8a50 CFRunLoopRun + 64
8   JavaScriptCore                	       0x1a47ecb50 WTF::Detail::CallableWrapper<WTF::RunLoop::create(WTF::ASCIILiteral, WTF::ThreadType, WTF::Thread::QOS)::$_0, void>::call() + 244
9   JavaScriptCore                	       0x1a483082c WTF::Thread::entryPoint(WTF::Thread::NewThreadContext*) + 260
10  JavaScriptCore                	       0x1a45fe28c WTF::wtfThreadEntryPoint(void*) + 16
11  libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
12  libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 58:

Thread 59:: Log work queue
0   libsystem_kernel.dylib        	       0x182ea4bb0 semaphore_wait_trap + 8
1   WebKit                        	       0x1ada87238 WTF::Detail::CallableWrapper<IPC::StreamConnectionWorkQueue::startProcessingThread()::$_0, void>::call() + 52
2   JavaScriptCore                	       0x1a483082c WTF::Thread::entryPoint(WTF::Thread::NewThreadContext*) + 260
3   JavaScriptCore                	       0x1a45fe28c WTF::wtfThreadEntryPoint(void*) + 16
4   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
5   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 60:

Thread 61::  Dispatch queue: com.apple.root.user-interactive-qos
0   libsystem_kernel.dylib        	       0x182ea4c34 mach_msg2_trap + 8
1   libsystem_kernel.dylib        	       0x182eb7028 mach_msg2_internal + 76
2   libsystem_kernel.dylib        	       0x182ead98c mach_msg_overwrite + 484
3   libsystem_kernel.dylib        	       0x182ea4fb4 mach_msg + 24
4   CoreFoundation                	       0x182f86bb0 __CFRunLoopServiceMachPort + 160
5   CoreFoundation                	       0x182f85508 __CFRunLoopRun + 1188
6   CoreFoundation                	       0x18303ff3c _CFRunLoopRunSpecificWithOptions + 532
7   Foundation                    	       0x1851d5d10 -[NSRunLoop(NSRunLoop) runMode:beforeDate:] + 212
8   AppKit                        	       0x1878ed0c4 -[NSAnimation _runBlocking] + 412
9   libdispatch.dylib             	       0x182d2ab5c _dispatch_call_block_and_release + 32
10  libdispatch.dylib             	       0x182d44ad4 _dispatch_client_callout + 16
11  libdispatch.dylib             	       0x182d619dc <deduplicated_symbol> + 32
12  libdispatch.dylib             	       0x182d3d13c _dispatch_root_queue_drain + 736
13  libdispatch.dylib             	       0x182d3d784 _dispatch_worker_thread2 + 180
14  libsystem_pthread.dylib       	       0x182ee3e10 _pthread_wqthread + 232
15  libsystem_pthread.dylib       	       0x182ee2b9c start_wqthread + 8

Thread 62:

Thread 63:

Thread 64:

Thread 65:

Thread 66:

Thread 67:

Thread 68:: JavaScriptCore libpas scavenger
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   JavaScriptCore                	       0x1a5e2fdd0 scavenger_thread_main + 1440
3   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
4   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 69::  Dispatch queue: com.apple.root.user-interactive-qos
0   libsystem_kernel.dylib        	       0x182ea4c34 mach_msg2_trap + 8
1   libsystem_kernel.dylib        	       0x182eb7028 mach_msg2_internal + 76
2   libsystem_kernel.dylib        	       0x182ead98c mach_msg_overwrite + 484
3   libsystem_kernel.dylib        	       0x182ea4fb4 mach_msg + 24
4   CoreFoundation                	       0x182f86bb0 __CFRunLoopServiceMachPort + 160
5   CoreFoundation                	       0x182f85508 __CFRunLoopRun + 1188
6   CoreFoundation                	       0x18303ff3c _CFRunLoopRunSpecificWithOptions + 532
7   Foundation                    	       0x1851d5d10 -[NSRunLoop(NSRunLoop) runMode:beforeDate:] + 212
8   AppKit                        	       0x1878ed0c4 -[NSAnimation _runBlocking] + 412
9   libdispatch.dylib             	       0x182d2ab5c _dispatch_call_block_and_release + 32
10  libdispatch.dylib             	       0x182d44ad4 _dispatch_client_callout + 16
11  libdispatch.dylib             	       0x182d619dc <deduplicated_symbol> + 32
12  libdispatch.dylib             	       0x182d3d13c _dispatch_root_queue_drain + 736
13  libdispatch.dylib             	       0x182d3d784 _dispatch_worker_thread2 + 180
14  libsystem_pthread.dylib       	       0x182ee3e10 _pthread_wqthread + 232
15  libsystem_pthread.dylib       	       0x182ee2b9c start_wqthread + 8

Thread 70:

Thread 71:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 72:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 73:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 74:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 75:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 76:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 77:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 78:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 79:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 80:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 81:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 82:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 83:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 84:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 85:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 86:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 87:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 88:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 89:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 90:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 91:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 92:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 93:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 94:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 95:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 96:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 97:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 98:: Pool
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee80dc _pthread_cond_wait + 984
2   libc++.1.dylib                	       0x182e187ec std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>) + 104
3   Splice INSTRUMENT             	       0x1112ea9bc 0x110a80000 + 8825276
4   Splice INSTRUMENT             	       0x1112b4d7c 0x110a80000 + 8605052
5   Splice INSTRUMENT             	       0x1112de200 0x110a80000 + 8774144
6   Splice INSTRUMENT             	       0x1112b573c 0x110a80000 + 8607548
7   Splice INSTRUMENT             	       0x1112e4144 0x110a80000 + 8798532
8   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
9   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 99:: CVDisplayLink
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee8108 _pthread_cond_wait + 1028
2   CoreVideo                     	       0x18d58ab3c CVDisplayLink::waitUntil(unsigned long long) + 336
3   CoreVideo                     	       0x18d589c24 CVDisplayLink::runIOThread() + 500
4   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
5   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 100:: CVDisplayLink
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee8108 _pthread_cond_wait + 1028
2   CoreVideo                     	       0x18d58ab3c CVDisplayLink::waitUntil(unsigned long long) + 336
3   CoreVideo                     	       0x18d589c24 CVDisplayLink::runIOThread() + 500
4   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
5   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 101:: CVDisplayLink
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee8108 _pthread_cond_wait + 1028
2   CoreVideo                     	       0x18d58ab3c CVDisplayLink::waitUntil(unsigned long long) + 336
3   CoreVideo                     	       0x18d589c24 CVDisplayLink::runIOThread() + 500
4   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
5   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8

Thread 102:: CVDisplayLink
0   libsystem_kernel.dylib        	       0x182ea84f8 __psynch_cvwait + 8
1   libsystem_pthread.dylib       	       0x182ee8108 _pthread_cond_wait + 1028
2   CoreVideo                     	       0x18d58ab3c CVDisplayLink::waitUntil(unsigned long long) + 336
3   CoreVideo                     	       0x18d589c24 CVDisplayLink::runIOThread() + 500
4   libsystem_pthread.dylib       	       0x182ee7c08 _pthread_start + 136
5   libsystem_pthread.dylib       	       0x182ee2ba8 thread_start + 8


Thread 0 crashed with ARM Thread State (64-bit):
    x0: 0x0000000c98711a58   x1: 0x0000000ca27bb000   x2: 0x00000000000120a9   x3: 0x0000000c94d6a948
    x4: 0x0000000000000008   x5: 0x0000000000000020   x6: 0x0000000c96cc46c0   x7: 0xfffff0003ffff800
    x8: 0x140f1f0ef9400000   x9: 0x00000000a1a1a1a1  x10: 0x0003c5000003c502  x11: 0x0000010000000000
   x12: 0x00000000fffffffd  x13: 0x0003c50000000000  x14: 0x000000000003c500  x15: 0x0000000000000000
   x16: 0x0000000182a9bde8  x17: 0x0003c5000003c502  x18: 0x0000000000000000  x19: 0x0000000c98711a58
   x20: 0x0000000c957d1a80  x21: 0x00000001112df554  x22: 0x00000000141300b9  x23: 0x00000001ef1cb000
   x24: 0x0000000000000002  x25: 0x0000000000000000  x26: 0x00000000211200d5  x27: 0x0000000000000000
   x28: 0x0000000000001203   fp: 0x000000016b801950   lr: 0x0000000111289944
    sp: 0x000000016b801900   pc: 0x0000000111062500 cpsr: 0x80000000
   far: 0x140f1f0ef9400010  esr: 0x92000004 (Data Abort) byte read Translation fault

Binary Images:
       0x1045fc000 -        0x104d17fff com.yourcompany.Pitchenga (1.0.0) <d37f7bd1-e877-35a4-b254-26c769c0234e> /Users/USER/*/Pitchenga.app/Contents/MacOS/Pitchenga
       0x110a80000 -        0x11208bfff com.splice.Instrument (1.7.0-c2c6d65) <16497075-975a-3e52-b670-57e5ab7d3ace> /Library/Audio/Plug-Ins/Components/Splice INSTRUMENT.component/Contents/MacOS/Splice INSTRUMENT
       0x10ddf4000 -        0x10df37fff com.apple.audio.units.Components (1.14) <d4431c82-73db-32cd-aa69-fb5df573e77e> /System/Library/Components/CoreAudio.component/Contents/MacOS/CoreAudio
       0x10dd44000 -        0x10dd4ffff libobjc-trampolines.dylib (*) <f26af954-d1f7-31aa-9981-1bad216149d9> /usr/lib/libobjc-trampolines.dylib
       0x12227c000 -        0x122ab3fff com.apple.AGXMetalG16X (345.20.4) <f3801a6f-cfb2-3ee3-a8a0-320185aa69b6> /System/Library/Extensions/AGXMetalG16X.bundle/Contents/MacOS/AGXMetalG16X
       0x10ffe8000 -        0x11004bfff com.apple.AppleMetalOpenGLRenderer (1.0) <66a35ba7-f2f4-3a44-9a0c-8440564e9578> /System/Library/Extensions/AppleMetalOpenGLRenderer.bundle/Contents/MacOS/AppleMetalOpenGLRenderer
       0x182f27000 -        0x1834702bf com.apple.CoreFoundation (6.9) <646518bb-a6e8-3da7-ab32-9d97bcbdc25d> /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
       0x18fa2f000 -        0x18fd3227f com.apple.HIToolbox (2.1.1) <3c068ca7-e6a9-3e91-953a-b527a1892d05> /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/HIToolbox
       0x187380000 -        0x188aac6ff com.apple.AppKit (6.9) <bf94fd49-283d-3c62-8cf0-0a49c21b6129> /System/Library/Frameworks/AppKit.framework/Versions/C/AppKit
       0x182b15000 -        0x182bb4713 dyld (*) <044cd67e-3a0a-3ca4-8bb3-a9687d5328fe> /usr/lib/dyld
               0x0 - 0xffffffffffffffff ??? (*) <00000000-0000-0000-0000-000000000000> ???
       0x182a90000 -        0x182ae34bb libobjc.A.dylib (*) <9e66cff2-3ebd-3242-8166-d5d0c204755b> /usr/lib/libobjc.A.dylib
       0x182ea4000 -        0x182ee049f libsystem_kernel.dylib (*) <78ec33a6-6330-3836-8900-eb90836936e8> /usr/lib/system/libsystem_kernel.dylib
       0x182ee1000 -        0x182eedacb libsystem_pthread.dylib (*) <0596a7b6-bce2-3f06-a2e8-3eaab5371ed8> /usr/lib/system/libsystem_pthread.dylib
       0x1a1e06000 -        0x1a1ed349f com.apple.audio.midi.CoreMIDI (2.0) <349e1842-7521-35a6-aca9-e80329ec86a8> /System/Library/Frameworks/CoreMIDI.framework/Versions/A/CoreMIDI
       0x18f577000 -        0x18f59fd7f com.apple.audio.caulk (1.0) <67d6bcc2-f024-388e-b546-b1c2fe6f0ddc> /System/Library/PrivateFrameworks/caulk.framework/Versions/A/caulk
       0x185ff8000 -        0x1867a939f com.apple.audio.CoreAudio (5.0) <16b7552a-ead3-374c-a642-5a11a7523189> /System/Library/Frameworks/CoreAudio.framework/Versions/A/CoreAudio
       0x182df6000 -        0x182e88ea3 libc++.1.dylib (*) <652836ca-32b1-3388-a72a-d6b90ddda958> /usr/lib/libc++.1.dylib
       0x182c30000 -        0x182cdcccf libcorecrypto.dylib (*) <a6fb5220-05f1-36a3-93ca-e45f15c98d3a> /usr/lib/system/libcorecrypto.dylib
       0x1a45f8000 -        0x1a602a29f com.apple.JavaScriptCore (21623) <fe18d222-b3fa-39cb-a89b-1001fa2d1878> /System/Library/Frameworks/JavaScriptCore.framework/Versions/A/JavaScriptCore
       0x1ac875000 -        0x1adeb7b5f com.apple.WebKit (21623) <c13188b8-0146-3f71-8e33-8f6808e4218a> /System/Library/Frameworks/WebKit.framework/Versions/A/WebKit
       0x18477a000 -        0x18571dc1f com.apple.Foundation (6.9) <187e7913-b154-30a7-8070-852767aac3cf> /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
       0x182d29000 -        0x182d6fe5f libdispatch.dylib (*) <4c58ab31-f363-3e75-a8f8-302105812dbf> /usr/lib/system/libdispatch.dylib
       0x18d587000 -        0x18d60b01f com.apple.CoreVideo (1.8) <67098f59-d15b-3748-bcb9-c8041ce75424> /System/Library/Frameworks/CoreVideo.framework/Versions/A/CoreVideo

External Modification Summary:
  Calls made by other processes targeting this process:
    task_for_pid: 0
    thread_create: 0
    thread_set_state: 0
  Calls made by this process:
    task_for_pid: 0
    thread_create: 0
    thread_set_state: 0
  Calls made by all processes on this machine:
    task_for_pid: 5
    thread_create: 0
    thread_set_state: 392

VM Region Summary:
ReadOnly portion of Libraries: Total=1.8G resident=0K(0%) swapped_out_or_unallocated=1.8G(100%)
Writable regions: Total=5.0G written=2259K(0%) resident=2259K(0%) swapped_out=0K(0%) unallocated=5.0G(100%)

                                VIRTUAL   REGION 
REGION TYPE                        SIZE    COUNT (non-coalesced) 
===========                     =======  ======= 
.note.gnu.proper                    320        1 
Accelerate framework               256K        2 
Activity Tracing                   256K        1 
AttributeGraph Data               1024K        1 
CG image                           432K        6 
ColorSync                           64K        4 
CoreAnimation                     12.6M       32 
CoreGraphics                       112K        7 
CoreUI image data                  320K        5 
Foundation                          16K        1 
Kernel Alloc Once                   32K        1 
MALLOC                           666.8M      154 
MALLOC guard page                 3328K        4 
MALLOC_LARGE (reserved)           8192K        1         reserved VM address space (unallocated)
STACK GUARD                       57.6M      103 
Stack                             62.2M      103 
VM_ALLOCATE                       1056K       26 
VM_ALLOCATE (reserved)             4.0G        2         reserved VM address space (unallocated)
WebKit Malloc                    256.1M        8 
__AUTH                            5873K      649 
__AUTH_CONST                      88.5M     1027 
__CTF                               824        1 
__DATA                            35.7M      984 
__DATA_CONST                      33.7M     1038 
__DATA_DIRTY                      8428K      885 
__FONT_DATA                        2352        1 
__GLSLBUILTINS                    5176K        1 
__INFO_FILTER                         8        1 
__LINKEDIT                       593.8M        7 
__OBJC_RO                         78.4M        1 
__OBJC_RW                         2571K        1 
__TEXT                             1.3G     1060 
__TPRO_CONST                       128K        2 
mapped file                        2.8G       65 
page table in kernel              2259K        1 
shared memory                     1568K       20 
===========                     =======  ======= 
TOTAL                             10.0G     6206 
TOTAL, minus reserved VM space     6.0G     6206 


-----------
Full Report
-----------

{"app_name":"Pitchenga","timestamp":"2026-04-25 18:51:04.00 +0200","app_version":"1.0.0","slice_uuid":"d37f7bd1-e877-35a4-b254-26c769c0234e","build_version":"1.0.0","platform":1,"bundleID":"com.yourcompany.Pitchenga","share_with_app_devs":1,"is_first_party":0,"bug_type":"309","os_version":"macOS 26.3.1 (25D2128)","roots_installed":0,"name":"Pitchenga","incident_id":"E45405C0-E2F2-4F2A-9100-AB7DB2DCC634"}
{
  "uptime" : 190000,
  "procRole" : "Foreground",
  "version" : 2,
  "userID" : 501,
  "deployVersion" : 210,
  "modelCode" : "Mac16,11",
  "coalitionID" : 56339,
  "osVersion" : {
    "train" : "macOS 26.3.1",
    "build" : "25D2128",
    "releaseType" : "User"
  },
  "captureTime" : "2026-04-25 18:50:54.5379 +0200",
  "codeSigningMonitor" : 2,
  "incident" : "E45405C0-E2F2-4F2A-9100-AB7DB2DCC634",
  "pid" : 93003,
  "translated" : false,
  "cpuType" : "ARM-64",
  "procLaunch" : "2026-04-25 18:49:55.6387 +0200",
  "procStartAbsTime" : 4603172795809,
  "procExitAbsTime" : 4604585819003,
  "procName" : "Pitchenga",
  "procPath" : "\/Users\/USER\/*\/Pitchenga.app\/Contents\/MacOS\/Pitchenga",
  "bundleInfo" : {"CFBundleShortVersionString":"1.0.0","CFBundleVersion":"1.0.0","CFBundleIdentifier":"com.yourcompany.Pitchenga"},
  "storeInfo" : {"deviceIdentifierForVendor":"741F023B-FDFB-59FA-BDF6-9126D0FB3D94","thirdParty":true},
  "parentProc" : "clion",
  "parentPid" : 70060,
  "coalitionName" : "com.jetbrains.CLion",
  "crashReporterKey" : "31EF1460-9124-D2FD-4CAC-4A806E474698",
  "appleIntelligenceStatus" : {"state":"unavailable","reasons":["selectedLanguageDoesNotMatchSelectedSiriLanguage","selectedLanguageDoesNotMatchSelectedSiriLanguageInfo(system: en-GB, siri: en)"]},
  "developerMode" : 1,
  "responsiblePid" : 70060,
  "responsibleProc" : "clion",
  "codeSigningID" : "Pitchenga",
  "codeSigningTeamID" : "",
  "codeSigningFlags" : 570556929,
  "codeSigningValidationCategory" : 10,
  "codeSigningTrustLevel" : 4294967295,
  "codeSigningAuxiliaryInfo" : 0,
  "instructionByteStream" : {"beforePC":"8wMAqkiBANAISUL5CAFA+egXAPkIBED5aAQAtBUJQPk1BAC0qAJA+Q==","atPC":"CAlA+eADFaoAAT\/WgAMANnRCApHoAwCR4AMUqhX\/\/5foA0D5CAFAOQ=="},
  "bootSessionUUID" : "7E46AF4C-667C-4384-927A-64CCD61FFEBF",
  "wakeTime" : 32894,
  "sleepWakeUUID" : "D4168D9B-1C8D-4B3E-BF3B-BB54A935B8C4",
  "sip" : "enabled",
  "vmRegionInfo" : "0x1f0ef9400010 is not in any region.  \n      REGION TYPE                    START - END         [ VSIZE] PRT\/MAX SHRMOD  REGION DETAIL\n      UNUSED SPACE AT START\n--->  \n      UNUSED SPACE AT END",
  "exception" : {"codes":"0x0000000000000001, 0x140f1f0ef9400010","rawCodes":[1,1445408154580942864],"type":"EXC_BAD_ACCESS","signal":"SIGSEGV","subtype":"KERN_INVALID_ADDRESS at 0x140f1f0ef9400010"},
  "termination" : {"flags":0,"code":11,"namespace":"SIGNAL","indicator":"Segmentation fault: 11","byProc":"exc handler","byPid":93003},
  "vmregioninfo" : "0x1f0ef9400010 is not in any region.  \n      REGION TYPE                    START - END         [ VSIZE] PRT\/MAX SHRMOD  REGION DETAIL\n      UNUSED SPACE AT START\n--->  \n      UNUSED SPACE AT END",
  "extMods" : {"caller":{"thread_create":0,"thread_set_state":0,"task_for_pid":0},"system":{"thread_create":0,"thread_set_state":392,"task_for_pid":5},"targeted":{"thread_create":0,"thread_set_state":0,"task_for_pid":0},"warnings":0},
  "faultingThread" : 0,
  "threads" : [{"threadState":{"x":[{"value":54097156696},{"value":54265622528},{"value":73897},{"value":54036703560},{"value":8},{"value":32},{"value":54069577408},{"value":18446726482597246976},{"value":1445408154580942848},{"value":2711724449},{"value":1061028721050882},{"value":1099511627776},{"value":4294967293},{"value":1061028720803840},{"value":247040},{"value":0},{"value":6487129576,"symbolLocation":0,"symbol":"objc_autoreleasePoolPush"},{"value":1061028721050882},{"value":0},{"value":54097156696},{"value":54047611520},{"value":4583191892},{"value":336789689},{"value":8306601984,"symbolLocation":728,"symbol":"_CFXNotificationPost.samples"},{"value":2},{"value":0},{"value":554827989},{"value":0},{"value":4611}],"flavor":"ARM_THREAD_STATE64","lr":{"value":4582840644},"cpsr":{"value":2147483648},"fp":{"value":6098524496},"sp":{"value":6098524416},"esr":{"value":2449473540,"description":"(Data Abort) byte read Translation fault"},"pc":{"value":4580582656,"matchesCrashFrame":1},"far":{"value":1445408154580942864}},"id":5528475,"triggered":true,"name":"JUCE v8.0.12: Message Thread","queue":"com.apple.main-thread","frames":[{"imageOffset":6169856,"imageIndex":1},{"imageOffset":8427844,"imageIndex":1},{"imageOffset":8427708,"imageIndex":1},{"imageOffset":391688,"symbol":"__CFRUNLOOP_IS_CALLING_OUT_TO_A_SOURCE0_PERFORM_FUNCTION__","symbolLocation":28,"imageIndex":6},{"imageOffset":391580,"symbol":"__CFRunLoopDoSource0","symbolLocation":172,"imageIndex":6},{"imageOffset":391020,"symbol":"__CFRunLoopDoSources0","symbolLocation":332,"imageIndex":6},{"imageOffset":385944,"symbol":"__CFRunLoopRun","symbolLocation":820,"imageIndex":6},{"imageOffset":1150780,"symbol":"_CFRunLoopRunSpecificWithOptions","symbolLocation":532,"imageIndex":6},{"imageOffset":792464,"symbol":"RunCurrentEventLoopInMode","symbolLocation":316,"imageIndex":7},{"imageOffset":805560,"symbol":"ReceiveNextEventCommon","symbolLocation":488,"imageIndex":7},{"imageOffset":2419556,"symbol":"_BlockUntilNextEventMatchingListInMode","symbolLocation":48,"imageIndex":7},{"imageOffset":5382164,"symbol":"_DPSBlockUntilNextEventMatchingListInMode","symbolLocation":236,"imageIndex":8},{"imageOffset":130632,"symbol":"_DPSNextEvent","symbolLocation":588,"imageIndex":8},{"imageOffset":11431688,"symbol":"-[NSApplication(NSEventRouting) _nextEventMatchingEventMask:untilDate:inMode:dequeue:]","symbolLocation":688,"imageIndex":8},{"imageOffset":11430932,"symbol":"-[NSApplication(NSEventRouting) nextEventMatchingMask:untilDate:inMode:dequeue:]","symbolLocation":72,"imageIndex":8},{"imageOffset":100224,"symbol":"-[NSApplication run]","symbolLocation":368,"imageIndex":8},{"imageOffset":3494620,"symbol":"juce::JUCEApplicationBase::main()","symbolLocation":192,"imageIndex":0},{"imageOffset":3494348,"symbol":"juce::JUCEApplicationBase::main(int, char const**)","symbolLocation":88,"imageIndex":0},{"imageOffset":36180,"symbol":"start","symbolLocation":7184,"imageIndex":9}]},{"id":5528520,"frames":[],"threadState":{"x":[{"value":6100807680},{"value":136707},{"value":6100271104},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6100807680},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5528522,"name":"com.apple.NSEventThread","threadState":{"x":[{"value":268451845},{"value":21592279046},{"value":8589934592,"objc-selector":"z"},{"value":98968931401728},{"value":0},{"value":98968931401728},{"value":2},{"value":4294967295},{"value":0},{"value":17179869184},{"value":0},{"value":2},{"value":0},{"value":0},{"value":23043},{"value":0},{"value":18446744073709551569},{"value":8329287384},{"value":0},{"value":4294967295},{"value":2},{"value":98968931401728},{"value":0},{"value":98968931401728},{"value":6101377160},{"value":8589934592,"objc-selector":"z"},{"value":21592279046},{"value":18446744073709550527},{"value":4412409862}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491435048},"cpsr":{"value":0},"fp":{"value":6101377008},"sp":{"value":6101376928},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491360308},"far":{"value":0}},"frames":[{"imageOffset":3124,"symbol":"mach_msg2_trap","symbolLocation":8,"imageIndex":12},{"imageOffset":77864,"symbol":"mach_msg2_internal","symbolLocation":76,"imageIndex":12},{"imageOffset":39308,"symbol":"mach_msg_overwrite","symbolLocation":484,"imageIndex":12},{"imageOffset":4020,"symbol":"mach_msg","symbolLocation":24,"imageIndex":12},{"imageOffset":392112,"symbol":"__CFRunLoopServiceMachPort","symbolLocation":160,"imageIndex":6},{"imageOffset":386312,"symbol":"__CFRunLoopRun","symbolLocation":1188,"imageIndex":6},{"imageOffset":1150780,"symbol":"_CFRunLoopRunSpecificWithOptions","symbolLocation":532,"imageIndex":6},{"imageOffset":719412,"symbol":"_NSEventThread","symbolLocation":184,"imageIndex":8},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528523,"frames":[{"imageOffset":3124,"symbol":"mach_msg2_trap","symbolLocation":8,"imageIndex":12},{"imageOffset":77864,"symbol":"mach_msg2_internal","symbolLocation":76,"imageIndex":12},{"imageOffset":39308,"symbol":"mach_msg_overwrite","symbolLocation":484,"imageIndex":12},{"imageOffset":4020,"symbol":"mach_msg","symbolLocation":24,"imageIndex":12},{"imageOffset":298792,"symbol":"XServerMachPort::ReceiveMessage(int&, void*, int&)","symbolLocation":104,"imageIndex":14},{"imageOffset":438844,"symbol":"MIDIProcess::MIDIInPortThread::Run()","symbolLocation":148,"imageIndex":14},{"imageOffset":389864,"symbol":"CADeprecated::XThread::RunHelper(void*)","symbolLocation":48,"imageIndex":14},{"imageOffset":436672,"symbol":"CADeprecated::CAPThread::Entry(void*)","symbolLocation":96,"imageIndex":14},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}],"threadState":{"x":[{"value":268451845},{"value":17179869186},{"value":0},{"value":112163070935040},{"value":0},{"value":112163070935040},{"value":100},{"value":0},{"value":0},{"value":17179869184},{"value":100},{"value":0},{"value":0},{"value":0},{"value":26115},{"value":6336957910031007744},{"value":18446744073709551569},{"value":8329287384},{"value":0},{"value":0},{"value":100},{"value":112163070935040},{"value":0},{"value":112163070935040},{"value":6101952980},{"value":0},{"value":17179869186},{"value":18446744073709550527},{"value":2}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491435048},"cpsr":{"value":0},"fp":{"value":6101952640},"sp":{"value":6101952560},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491360308},"far":{"value":0}}},{"id":5528526,"name":"caulk.messenger.shared:17","threadState":{"x":[{"value":14},{"value":1},{"value":0},{"value":1},{"value":0},{"value":1},{"value":0},{"value":0},{"value":0},{"value":4294967295},{"value":0},{"value":0},{"value":4525834264},{"value":6102527576},{"value":7},{"value":0},{"value":18446744073709551580},{"value":8329289888},{"value":0},{"value":54069280800},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6699847176},"cpsr":{"value":2147483648},"fp":{"value":6102527872},"sp":{"value":6102527840},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491360176},"far":{"value":0}},"frames":[{"imageOffset":2992,"symbol":"semaphore_wait_trap","symbolLocation":8,"imageIndex":12},{"imageOffset":7688,"symbol":"caulk::semaphore::timed_wait(double)","symbolLocation":224,"imageIndex":15},{"imageOffset":7344,"symbol":"caulk::concurrent::details::worker_thread::run()","symbolLocation":32,"imageIndex":15},{"imageOffset":6480,"symbol":"void* caulk::thread_proxy<std::__1::tuple<caulk::thread::attributes, void (caulk::concurrent::details::worker_thread::*)(), std::__1::tuple<caulk::concurrent::details::worker_thread*>>>(void*)","symbolLocation":96,"imageIndex":15},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528527,"name":"caulk.messenger.shared:high","threadState":{"x":[{"value":14},{"value":43011},{"value":43011},{"value":53},{"value":4294967295},{"value":0},{"value":0},{"value":0},{"value":0},{"value":4294967295},{"value":1},{"value":54081397576},{"value":0},{"value":0},{"value":0},{"value":0},{"value":18446744073709551580},{"value":8329289888},{"value":0},{"value":54069281472},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6699847176},"cpsr":{"value":2147483648},"fp":{"value":6103101312},"sp":{"value":6103101280},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491360176},"far":{"value":0}},"frames":[{"imageOffset":2992,"symbol":"semaphore_wait_trap","symbolLocation":8,"imageIndex":12},{"imageOffset":7688,"symbol":"caulk::semaphore::timed_wait(double)","symbolLocation":224,"imageIndex":15},{"imageOffset":7344,"symbol":"caulk::concurrent::details::worker_thread::run()","symbolLocation":32,"imageIndex":15},{"imageOffset":6480,"symbol":"void* caulk::thread_proxy<std::__1::tuple<caulk::thread::attributes, void (caulk::concurrent::details::worker_thread::*)(), std::__1::tuple<caulk::concurrent::details::worker_thread*>>>(void*)","symbolLocation":96,"imageIndex":15},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528571,"name":"com.apple.audio.IOThread.client","threadState":{"x":[{"value":0},{"value":122883},{"value":0},{"value":0},{"value":0},{"value":8},{"value":57},{"value":18446744073709551615},{"value":1},{"value":14677189380237492421},{"value":1099511628032},{"value":1099511628034},{"value":48},{"value":0},{"value":0},{"value":0},{"value":18446744073709551579},{"value":8329289896},{"value":0},{"value":54061378872},{"value":54061378864},{"value":54061378896},{"value":1},{"value":54069550240},{"value":177},{"value":0},{"value":6550463717},{"value":54061378864},{"value":54061378048}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6699966380},"cpsr":{"value":1610612736},"fp":{"value":6103674112},"sp":{"value":6103674096},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491360188},"far":{"value":0}},"frames":[{"imageOffset":3004,"symbol":"semaphore_wait_signal_trap","symbolLocation":8,"imageIndex":12},{"imageOffset":126892,"symbol":"caulk::mach::semaphore::wait_signal_or_error(caulk::mach::semaphore&)","symbolLocation":36,"imageIndex":15},{"imageOffset":2135756,"symbol":"HALC_ProxyIOContext::IOWorkLoop()","symbolLocation":5052,"imageIndex":16},{"imageOffset":2129000,"symbol":"invocation function for block in HALC_ProxyIOContext::HALC_ProxyIOContext(unsigned int, unsigned int)","symbolLocation":172,"imageIndex":16},{"imageOffset":3985164,"symbol":"HALC_IOThread::Entry(void*)","symbolLocation":88,"imageIndex":16},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528603,"name":"com.apple.audio.IOThread.client","threadState":{"x":[{"value":0},{"value":121091},{"value":0},{"value":0},{"value":0},{"value":16},{"value":6104820800},{"value":18446726482597246976},{"value":1},{"value":14677189380237492421},{"value":1099511628032},{"value":1099511628034},{"value":48},{"value":0},{"value":0},{"value":0},{"value":18446744073709551579},{"value":8329289896},{"value":0},{"value":54070494008},{"value":54070494000},{"value":54070494032},{"value":1},{"value":54069550624},{"value":174755},{"value":0},{"value":6550463717},{"value":54070494000},{"value":54070493184}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6699966380},"cpsr":{"value":1610612736},"fp":{"value":6104820992},"sp":{"value":6104820976},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491360188},"far":{"value":0}},"frames":[{"imageOffset":3004,"symbol":"semaphore_wait_signal_trap","symbolLocation":8,"imageIndex":12},{"imageOffset":126892,"symbol":"caulk::mach::semaphore::wait_signal_or_error(caulk::mach::semaphore&)","symbolLocation":36,"imageIndex":15},{"imageOffset":2135756,"symbol":"HALC_ProxyIOContext::IOWorkLoop()","symbolLocation":5052,"imageIndex":16},{"imageOffset":2129000,"symbol":"invocation function for block in HALC_ProxyIOContext::HALC_ProxyIOContext(unsigned int, unsigned int)","symbolLocation":172,"imageIndex":16},{"imageOffset":3985164,"symbol":"HALC_IOThread::Entry(void*)","symbolLocation":88,"imageIndex":16},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528618,"name":"LockFreeLogger","threadState":{"x":[{"value":260},{"value":0},{"value":309248},{"value":0},{"value":0},{"value":160},{"value":0},{"value":10000000},{"value":6105394728},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54080408240},{"value":54080408304},{"value":6105395424},{"value":10000000},{"value":0},{"value":309248},{"value":396545},{"value":396800},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6105394848},"sp":{"value":6105394704},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":4668120,"imageIndex":1},{"imageOffset":4591104,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528619,"name":"JUCE v8.0.8: Timer","threadState":{"x":[{"value":260},{"value":0},{"value":1229312},{"value":0},{"value":0},{"value":160},{"value":0},{"value":299999917},{"value":6105968088},{"value":0},{"value":15104},{"value":64871186053890},{"value":64871186053890},{"value":15104},{"value":0},{"value":64871186053888},{"value":305},{"value":8329285496},{"value":0},{"value":54061007424},{"value":54061007488},{"value":6105968864},{"value":299999917},{"value":0},{"value":1229312},{"value":1229569},{"value":1229824},{"value":4599234560},{"value":4294967295}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6105968208},"sp":{"value":6105968064},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8418740,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528657,"name":"Work Thread 0","threadState":{"x":[{"value":260},{"value":0},{"value":615424},{"value":0},{"value":0},{"value":160},{"value":0},{"value":21332291},{"value":6106541576},{"value":0},{"value":256},{"value":1099511628034},{"value":1099511628034},{"value":256},{"value":0},{"value":1099511628032},{"value":305},{"value":8329285496},{"value":0},{"value":54080410160},{"value":54080410224},{"value":6106542304},{"value":21332291},{"value":0},{"value":615424},{"value":616193},{"value":616448},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6106541696},"sp":{"value":6106541552},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":4668120,"imageIndex":1},{"imageOffset":4162692,"imageIndex":1},{"imageOffset":4591104,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528658,"name":"Work Thread 1","threadState":{"x":[{"value":260},{"value":0},{"value":228864},{"value":0},{"value":0},{"value":160},{"value":0},{"value":0},{"value":6107115064},{"value":0},{"value":1536},{"value":6597069768194},{"value":6597069768194},{"value":1536},{"value":0},{"value":6597069768192},{"value":305},{"value":8329285496},{"value":0},{"value":54080410320},{"value":54080410384},{"value":6107115744},{"value":0},{"value":0},{"value":228864},{"value":228865},{"value":229120},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6107115184},"sp":{"value":6107115040},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141140,"symbol":"std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&)","symbolLocation":32,"imageIndex":17},{"imageOffset":4668084,"imageIndex":1},{"imageOffset":4591104,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528659,"name":"Work Thread 2","threadState":{"x":[{"value":260},{"value":0},{"value":228608},{"value":0},{"value":0},{"value":160},{"value":0},{"value":0},{"value":6107688504},{"value":0},{"value":1024},{"value":4398046512130},{"value":4398046512130},{"value":1024},{"value":0},{"value":4398046512128},{"value":305},{"value":8329285496},{"value":0},{"value":54080410480},{"value":54080410544},{"value":6107689184},{"value":0},{"value":0},{"value":228608},{"value":228609},{"value":228864},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6107688624},"sp":{"value":6107688480},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141140,"symbol":"std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&)","symbolLocation":32,"imageIndex":17},{"imageOffset":4668084,"imageIndex":1},{"imageOffset":4591104,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528660,"name":"Work Thread 3","threadState":{"x":[{"value":260},{"value":0},{"value":228608},{"value":0},{"value":0},{"value":160},{"value":0},{"value":0},{"value":6108261944},{"value":0},{"value":1024},{"value":4398046512130},{"value":4398046512130},{"value":1024},{"value":0},{"value":4398046512128},{"value":305},{"value":8329285496},{"value":0},{"value":54080410640},{"value":54080410704},{"value":6108262624},{"value":0},{"value":0},{"value":228608},{"value":228609},{"value":228864},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6108262064},"sp":{"value":6108261920},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141140,"symbol":"std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&)","symbolLocation":32,"imageIndex":17},{"imageOffset":4668084,"imageIndex":1},{"imageOffset":4591104,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528661,"name":"Work Thread 4","threadState":{"x":[{"value":260},{"value":0},{"value":228608},{"value":0},{"value":0},{"value":160},{"value":0},{"value":0},{"value":6108835384},{"value":0},{"value":256},{"value":1099511628034},{"value":1099511628034},{"value":256},{"value":0},{"value":1099511628032},{"value":305},{"value":8329285496},{"value":0},{"value":54080410800},{"value":54080410864},{"value":6108836064},{"value":0},{"value":0},{"value":228608},{"value":228609},{"value":228864},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6108835504},"sp":{"value":6108835360},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141140,"symbol":"std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&)","symbolLocation":32,"imageIndex":17},{"imageOffset":4668084,"imageIndex":1},{"imageOffset":4591104,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528662,"name":"Work Thread 5","threadState":{"x":[{"value":4},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":0},{"value":6109408728},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54070303952},{"value":54070304016},{"value":6109409504},{"value":0},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6109408848},"sp":{"value":6109408704},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141140,"symbol":"std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&)","symbolLocation":32,"imageIndex":17},{"imageOffset":4668084,"imageIndex":1},{"imageOffset":1200516,"imageIndex":1},{"imageOffset":4591104,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528663,"name":"Work Thread 6","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":0},{"value":6109982168},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54070303952},{"value":54070304016},{"value":6109982944},{"value":0},{"value":0},{"value":0},{"value":0},{"value":512},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6109982288},"sp":{"value":6109982144},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141140,"symbol":"std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&)","symbolLocation":32,"imageIndex":17},{"imageOffset":4668084,"imageIndex":1},{"imageOffset":1200516,"imageIndex":1},{"imageOffset":4591104,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528664,"name":"WaveFS","threadState":{"x":[{"value":260},{"value":0},{"value":3840},{"value":0},{"value":0},{"value":160},{"value":0},{"value":0},{"value":6110555704},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54080410000},{"value":54080410064},{"value":6110556384},{"value":0},{"value":0},{"value":3840},{"value":3841},{"value":4096},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6110555824},"sp":{"value":6110555680},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141140,"symbol":"std::__1::condition_variable::wait(std::__1::unique_lock<std::__1::mutex>&)","symbolLocation":32,"imageIndex":17},{"imageOffset":4668084,"imageIndex":1},{"imageOffset":4591104,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528665,"name":"Controller","threadState":{"x":[{"value":316},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":1332292},{"value":6111129064},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54080411280},{"value":54080411344},{"value":6111129824},{"value":1332292},{"value":0},{"value":0},{"value":9036545},{"value":9036800},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6111129184},"sp":{"value":6111129040},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":4668120,"imageIndex":1},{"imageOffset":1147892,"imageIndex":1},{"imageOffset":4591104,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528666,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":28928},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998959},{"value":6111702536},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056585000},{"value":54056585064},{"value":6111703264},{"value":499998959},{"value":0},{"value":28928},{"value":28929},{"value":29184},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6111702656},"sp":{"value":6111702512},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528667,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":30976},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999875},{"value":6112275976},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056585512},{"value":54056585576},{"value":6112276704},{"value":499999875},{"value":0},{"value":30976},{"value":30977},{"value":31232},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6112276096},"sp":{"value":6112275952},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528668,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":34304},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998917},{"value":6112849416},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056586024},{"value":54056586088},{"value":6112850144},{"value":499998917},{"value":0},{"value":34304},{"value":34305},{"value":34560},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6112849536},"sp":{"value":6112849392},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528669,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":37376},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998875},{"value":6113422856},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056586536},{"value":54056586600},{"value":6113423584},{"value":499998875},{"value":0},{"value":37376},{"value":37377},{"value":37632},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6113422976},"sp":{"value":6113422832},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528670,"name":"Pool","threadState":{"x":[{"value":4},{"value":0},{"value":156},{"value":874722},{"value":0},{"value":4586590344},{"value":1},{"value":54041809536},{"value":156},{"value":0},{"value":24},{"value":1},{"value":0},{"value":0},{"value":0},{"value":0},{"value":230},{"value":8329287256},{"value":0},{"value":6113994800},{"value":2},{"value":0},{"value":0},{"value":1},{"value":0},{"value":156},{"value":54262733736},{"value":1},{"value":1}],"flavor":"ARM_THREAD_STATE64","lr":{"value":4586878420},"cpsr":{"value":536870912},"fp":{"value":6113994640},"sp":{"value":6113994608},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491395776},"far":{"value":0}},"frames":[{"imageOffset":38592,"symbol":"poll","symbolLocation":8,"imageIndex":12},{"imageOffset":12465620,"imageIndex":1},{"imageOffset":12418680,"imageIndex":1},{"imageOffset":12260480,"imageIndex":1},{"imageOffset":12074840,"imageIndex":1},{"imageOffset":12005448,"imageIndex":1},{"imageOffset":11999488,"imageIndex":1},{"imageOffset":11995100,"imageIndex":1},{"imageOffset":12016064,"imageIndex":1},{"imageOffset":12016864,"imageIndex":1},{"imageOffset":8615792,"imageIndex":1},{"imageOffset":8774128,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528671,"name":"Pool","threadState":{"x":[{"value":4},{"value":0},{"value":155},{"value":875142},{"value":0},{"value":4586590344},{"value":1},{"value":6488861120},{"value":155},{"value":0},{"value":26},{"value":1},{"value":0},{"value":0},{"value":0},{"value":0},{"value":230},{"value":8329287256},{"value":0},{"value":6114568240},{"value":2},{"value":0},{"value":0},{"value":1},{"value":0},{"value":155},{"value":54262732456},{"value":1},{"value":1}],"flavor":"ARM_THREAD_STATE64","lr":{"value":4586878420},"cpsr":{"value":536870912},"fp":{"value":6114568080},"sp":{"value":6114568048},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491395776},"far":{"value":0}},"frames":[{"imageOffset":38592,"symbol":"poll","symbolLocation":8,"imageIndex":12},{"imageOffset":12465620,"imageIndex":1},{"imageOffset":12418680,"imageIndex":1},{"imageOffset":12260480,"imageIndex":1},{"imageOffset":12074840,"imageIndex":1},{"imageOffset":12005448,"imageIndex":1},{"imageOffset":11999488,"imageIndex":1},{"imageOffset":11995100,"imageIndex":1},{"imageOffset":12016064,"imageIndex":1},{"imageOffset":12016864,"imageIndex":1},{"imageOffset":8615792,"imageIndex":1},{"imageOffset":8774128,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528672,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":44032},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6115143176},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056588072},{"value":54056588136},{"value":6115143904},{"value":499999958},{"value":0},{"value":44032},{"value":44033},{"value":44288},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6115143296},"sp":{"value":6115143152},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528673,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":45824},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6115716616},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056588584},{"value":54056588648},{"value":6115717344},{"value":499999958},{"value":0},{"value":45824},{"value":45825},{"value":46080},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6115716736},"sp":{"value":6115716592},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528674,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":48896},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998959},{"value":6116290056},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056589096},{"value":54056589160},{"value":6116290784},{"value":499998959},{"value":0},{"value":48896},{"value":48897},{"value":49152},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6116290176},"sp":{"value":6116290032},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528675,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":51200},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998958},{"value":6116863496},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056589608},{"value":54056589672},{"value":6116864224},{"value":499998958},{"value":0},{"value":51200},{"value":51201},{"value":51456},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6116863616},"sp":{"value":6116863472},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528676,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":52480},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6117436936},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056590120},{"value":54056590184},{"value":6117437664},{"value":499999958},{"value":0},{"value":52480},{"value":52481},{"value":52736},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6117437056},"sp":{"value":6117436912},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528677,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":53504},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998958},{"value":6118010376},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056590632},{"value":54056590696},{"value":6118011104},{"value":499998958},{"value":0},{"value":53504},{"value":53505},{"value":53760},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6118010496},"sp":{"value":6118010352},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528678,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":51968},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":6118583816},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056591144},{"value":54056591208},{"value":6118584544},{"value":499999959},{"value":0},{"value":51968},{"value":51969},{"value":52224},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6118583936},"sp":{"value":6118583792},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528679,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":56320},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6119157256},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056591656},{"value":54056591720},{"value":6119157984},{"value":499999958},{"value":0},{"value":56320},{"value":56321},{"value":56576},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6119157376},"sp":{"value":6119157232},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528680,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6119730696},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056592168},{"value":54056592232},{"value":6119731424},{"value":499999958},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6119730816},"sp":{"value":6119730672},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528681,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999875},{"value":6120304136},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056592680},{"value":54056592744},{"value":6120304864},{"value":499999875},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6120304256},"sp":{"value":6120304112},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528682,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6120877576},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056593192},{"value":54056593256},{"value":6120878304},{"value":499999958},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6120877696},"sp":{"value":6120877552},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528683,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998958},{"value":6121451016},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056593704},{"value":54056593768},{"value":6121451744},{"value":499998958},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6121451136},"sp":{"value":6121450992},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528684,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6122024456},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056594216},{"value":54056594280},{"value":6122025184},{"value":499999958},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6122024576},"sp":{"value":6122024432},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528685,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998959},{"value":6122597896},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056594728},{"value":54056594792},{"value":6122598624},{"value":499998959},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6122598016},"sp":{"value":6122597872},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528686,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998875},{"value":6123171336},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056595240},{"value":54056595304},{"value":6123172064},{"value":499998875},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6123171456},"sp":{"value":6123171312},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528687,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6123744776},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056595752},{"value":54056595816},{"value":6123745504},{"value":499999958},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6123744896},"sp":{"value":6123744752},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528688,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":6124318216},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056596264},{"value":54056596328},{"value":6124318944},{"value":499999959},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6124318336},"sp":{"value":6124318192},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528689,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998958},{"value":6124891656},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056596776},{"value":54056596840},{"value":6124892384},{"value":499998958},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6124891776},"sp":{"value":6124891632},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528690,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999917},{"value":6125465096},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056597288},{"value":54056597352},{"value":6125465824},{"value":499999917},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6125465216},"sp":{"value":6125465072},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528691,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998958},{"value":6126038536},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056597800},{"value":54056597864},{"value":6126039264},{"value":499998958},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6126038656},"sp":{"value":6126038512},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528692,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998875},{"value":6126611976},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056598312},{"value":54056598376},{"value":6126612704},{"value":499998875},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6126612096},"sp":{"value":6126611952},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528693,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998959},{"value":6127185416},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056598824},{"value":54056598888},{"value":6127186144},{"value":499998959},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6127185536},"sp":{"value":6127185392},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528694,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":28416},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998958},{"value":6127758856},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056599336},{"value":54056599400},{"value":6127759584},{"value":499998958},{"value":0},{"value":28416},{"value":29697},{"value":29952},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6127758976},"sp":{"value":6127758832},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528695,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998959},{"value":6128332296},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54062180136},{"value":54062180200},{"value":6128333024},{"value":499998959},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6128332416},"sp":{"value":6128332272},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528696,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":6128905736},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54062178600},{"value":54062178664},{"value":6128906464},{"value":499999959},{"value":0},{"value":0},{"value":29441},{"value":29696},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6128905856},"sp":{"value":6128905712},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528710,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":26624},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998959},{"value":6130052616},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54062178088},{"value":54062178152},{"value":6130053344},{"value":499998959},{"value":0},{"value":26624},{"value":29697},{"value":29952},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6130052736},"sp":{"value":6130052592},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528713,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":6129479176},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54056567080},{"value":54056567144},{"value":6129479904},{"value":499999959},{"value":0},{"value":0},{"value":29185},{"value":29440},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6129479296},"sp":{"value":6129479152},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528739,"name":"JUCE v8.0.12: Timer","threadState":{"x":[{"value":260},{"value":0},{"value":607232},{"value":0},{"value":0},{"value":160},{"value":0},{"value":299998959},{"value":6130625992},{"value":0},{"value":2560},{"value":10995116280322},{"value":10995116280322},{"value":2560},{"value":0},{"value":10995116280320},{"value":305},{"value":8329285496},{"value":0},{"value":54070269760},{"value":54070269824},{"value":6130626784},{"value":299998959},{"value":0},{"value":607232},{"value":607489},{"value":607744},{"value":4643985272004935680},{"value":4376170496,"symbolLocation":248,"symbol":"_MergedGlobals"}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6130626112},"sp":{"value":6130625968},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":4293100,"symbol":"std::__1::cv_status std::__1::condition_variable::wait_until[abi:nqe210106]<std::__1::chrono::steady_clock, std::__1::chrono::duration<double, std::__1::ratio<1l, 1000000000l>>>(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::steady_clock, std::__1::chrono::duration<double, std::__1::ratio<1l, 1000000000l>>> const&)","symbolLocation":276,"imageIndex":0},{"imageOffset":3885584,"symbol":"juce::WaitableEvent::wait(double) const","symbolLocation":236,"imageIndex":0},{"imageOffset":3530220,"symbol":"juce::Timer::TimerThread::run()","symbolLocation":460,"imageIndex":0},{"imageOffset":3888356,"symbol":"juce::Thread::threadEntryPoint()","symbolLocation":292,"imageIndex":0},{"imageOffset":4250228,"symbol":"juce::Thread::createNativeThread(juce::Thread::Priority)::$_0::__invoke(void*)","symbolLocation":156,"imageIndex":0},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528746,"name":"VisualizeWorker","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":1998958},{"value":6131199432},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54263759552},{"value":54263759616},{"value":6131200224},{"value":1998958},{"value":0},{"value":0},{"value":5647617},{"value":5647872},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6131199552},"sp":{"value":6131199408},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":4293100,"symbol":"std::__1::cv_status std::__1::condition_variable::wait_until[abi:nqe210106]<std::__1::chrono::steady_clock, std::__1::chrono::duration<double, std::__1::ratio<1l, 1000000000l>>>(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::steady_clock, std::__1::chrono::duration<double, std::__1::ratio<1l, 1000000000l>>> const&)","symbolLocation":276,"imageIndex":0},{"imageOffset":3885584,"symbol":"juce::WaitableEvent::wait(double) const","symbolLocation":236,"imageIndex":0},{"imageOffset":151512,"symbol":"Math::run()","symbolLocation":440,"imageIndex":0},{"imageOffset":3888356,"symbol":"juce::Thread::threadEntryPoint()","symbolLocation":292,"imageIndex":0},{"imageOffset":4250228,"symbol":"juce::Thread::createNativeThread(juce::Thread::Priority)::$_0::__invoke(void*)","symbolLocation":156,"imageIndex":0},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5528752,"frames":[],"threadState":{"x":[{"value":6132920320},{"value":136963},{"value":6132383744},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6132920320},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5528759,"frames":[],"threadState":{"x":[{"value":6144962560},{"value":165379},{"value":6144425984},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6144962560},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5529720,"frames":[],"threadState":{"x":[{"value":6104248320},{"value":241411},{"value":6103711744},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6104248320},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5530014,"name":"WebCore: Scrolling","threadState":{"x":[{"value":268451845},{"value":21592279046},{"value":8589934592,"objc-selector":"z"},{"value":801556861550592},{"value":0},{"value":801556861550592},{"value":2},{"value":4294967295},{"value":0},{"value":17179869184},{"value":0},{"value":2},{"value":0},{"value":0},{"value":186627},{"value":0},{"value":18446744073709551569},{"value":8329287384},{"value":0},{"value":4294967295},{"value":2},{"value":801556861550592},{"value":0},{"value":801556861550592},{"value":6416785352},{"value":8589934592,"objc-selector":"z"},{"value":21592279046},{"value":18446744073709550527},{"value":4412409862}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491435048},"cpsr":{"value":0},"fp":{"value":6416785200},"sp":{"value":6416785120},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491360308},"far":{"value":0}},"frames":[{"imageOffset":3124,"symbol":"mach_msg2_trap","symbolLocation":8,"imageIndex":12},{"imageOffset":77864,"symbol":"mach_msg2_internal","symbolLocation":76,"imageIndex":12},{"imageOffset":39308,"symbol":"mach_msg_overwrite","symbolLocation":484,"imageIndex":12},{"imageOffset":4020,"symbol":"mach_msg","symbolLocation":24,"imageIndex":12},{"imageOffset":392112,"symbol":"__CFRunLoopServiceMachPort","symbolLocation":160,"imageIndex":6},{"imageOffset":386312,"symbol":"__CFRunLoopRun","symbolLocation":1188,"imageIndex":6},{"imageOffset":1150780,"symbol":"_CFRunLoopRunSpecificWithOptions","symbolLocation":532,"imageIndex":6},{"imageOffset":727632,"symbol":"CFRunLoopRun","symbolLocation":64,"imageIndex":6},{"imageOffset":2050896,"symbol":"WTF::Detail::CallableWrapper<WTF::RunLoop::create(WTF::ASCIILiteral, WTF::ThreadType, WTF::Thread::QOS)::$_0, void>::call()","symbolLocation":244,"imageIndex":19},{"imageOffset":2328620,"symbol":"WTF::Thread::entryPoint(WTF::Thread::NewThreadContext*)","symbolLocation":260,"imageIndex":19},{"imageOffset":25228,"symbol":"WTF::wtfThreadEntryPoint(void*)","symbolLocation":16,"imageIndex":19},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5530101,"frames":[],"threadState":{"x":[{"value":6419083264},{"value":250795},{"value":6418546688},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6419083264},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5530119,"name":"Log work queue","threadState":{"x":[{"value":14},{"value":4569093648},{"value":372},{"value":5},{"value":8306486896,"symbolLocation":48,"symbol":"_OS_dispatch_queue_serial_vtable"},{"value":0},{"value":18446744072631617535},{"value":18446726482597246976},{"value":0},{"value":61},{"value":62},{"value":99},{"value":16},{"value":0},{"value":0},{"value":2},{"value":18446744073709551580},{"value":8329289888},{"value":0},{"value":5955912832},{"value":5955912872},{"value":6420230144},{"value":0},{"value":0},{"value":5955912960},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":7208464952},"cpsr":{"value":2147483648},"fp":{"value":6420229968},"sp":{"value":6420229936},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491360176},"far":{"value":0}},"frames":[{"imageOffset":2992,"symbol":"semaphore_wait_trap","symbolLocation":8,"imageIndex":12},{"imageOffset":18948664,"symbol":"WTF::Detail::CallableWrapper<IPC::StreamConnectionWorkQueue::startProcessingThread()::$_0, void>::call()","symbolLocation":52,"imageIndex":20},{"imageOffset":2328620,"symbol":"WTF::Thread::entryPoint(WTF::Thread::NewThreadContext*)","symbolLocation":260,"imageIndex":19},{"imageOffset":25228,"symbol":"WTF::wtfThreadEntryPoint(void*)","symbolLocation":16,"imageIndex":19},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5531266,"frames":[],"threadState":{"x":[{"value":6099087360},{"value":8347},{"value":6098550784},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6099087360},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5531268,"threadState":{"x":[{"value":268451845},{"value":21592279046},{"value":8589934592,"objc-selector":"z"},{"value":864263384072192},{"value":0},{"value":864263384072192},{"value":2},{"value":4294967295},{"value":0},{"value":17179869184},{"value":0},{"value":2},{"value":0},{"value":0},{"value":201227},{"value":0},{"value":18446744073709551569},{"value":8329287384},{"value":0},{"value":4294967295},{"value":2},{"value":864263384072192},{"value":0},{"value":864263384072192},{"value":6100229848},{"value":8589934592,"objc-selector":"z"},{"value":21592279046},{"value":18446744073709550527},{"value":4412409862}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491435048},"cpsr":{"value":0},"fp":{"value":6100229696},"sp":{"value":6100229616},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491360308},"far":{"value":0}},"queue":"com.apple.root.user-interactive-qos","frames":[{"imageOffset":3124,"symbol":"mach_msg2_trap","symbolLocation":8,"imageIndex":12},{"imageOffset":77864,"symbol":"mach_msg2_internal","symbolLocation":76,"imageIndex":12},{"imageOffset":39308,"symbol":"mach_msg_overwrite","symbolLocation":484,"imageIndex":12},{"imageOffset":4020,"symbol":"mach_msg","symbolLocation":24,"imageIndex":12},{"imageOffset":392112,"symbol":"__CFRunLoopServiceMachPort","symbolLocation":160,"imageIndex":6},{"imageOffset":386312,"symbol":"__CFRunLoopRun","symbolLocation":1188,"imageIndex":6},{"imageOffset":1150780,"symbol":"_CFRunLoopRunSpecificWithOptions","symbolLocation":532,"imageIndex":6},{"imageOffset":10861840,"symbol":"-[NSRunLoop(NSRunLoop) runMode:beforeDate:]","symbolLocation":212,"imageIndex":21},{"imageOffset":5689540,"symbol":"-[NSAnimation _runBlocking]","symbolLocation":412,"imageIndex":8},{"imageOffset":7004,"symbol":"_dispatch_call_block_and_release","symbolLocation":32,"imageIndex":22},{"imageOffset":113364,"symbol":"_dispatch_client_callout","symbolLocation":16,"imageIndex":22},{"imageOffset":231900,"symbol":"<deduplicated_symbol>","symbolLocation":32,"imageIndex":22},{"imageOffset":82236,"symbol":"_dispatch_root_queue_drain","symbolLocation":736,"imageIndex":22},{"imageOffset":83844,"symbol":"_dispatch_worker_thread2","symbolLocation":180,"imageIndex":22},{"imageOffset":11792,"symbol":"_pthread_wqthread","symbolLocation":232,"imageIndex":13},{"imageOffset":7068,"symbol":"start_wqthread","symbolLocation":8,"imageIndex":13}]},{"id":5531269,"frames":[],"threadState":{"x":[{"value":6131773440},{"value":19987},{"value":6131236864},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6131773440},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5531270,"frames":[],"threadState":{"x":[{"value":6141521920},{"value":259335},{"value":6140985344},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6141521920},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5531271,"frames":[],"threadState":{"x":[{"value":6142095360},{"value":176915},{"value":6141558784},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6142095360},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5531272,"frames":[],"threadState":{"x":[{"value":6142668800},{"value":259595},{"value":6142132224},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6142668800},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5531273,"frames":[],"threadState":{"x":[{"value":6143242240},{"value":137843},{"value":6142705664},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6143242240},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5531298,"frames":[],"threadState":{"x":[{"value":6143815680},{"value":176507},{"value":6143279104},{"value":0},{"value":409604},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6143815680},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5531347,"name":"JavaScriptCore libpas scavenger","threadState":{"x":[{"value":260},{"value":0},{"value":166400},{"value":0},{"value":0},{"value":160},{"value":0},{"value":4999032},{"value":6144388776},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":14208854080},{"value":14208854144},{"value":6144389344},{"value":4999032},{"value":0},{"value":166400},{"value":170241},{"value":170496},{"value":8274956288,"symbolLocation":4716912,"symbol":"pas_mar_global_registry"},{"value":8309104640,"symbolLocation":3400,"symbol":"bmalloc_common_primitive_heap_support"}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6144388896},"sp":{"value":6144388752},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":25394640,"symbol":"scavenger_thread_main","symbolLocation":1440,"imageIndex":19},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5531564,"threadState":{"x":[{"value":268451845},{"value":21592279046},{"value":8589934592,"objc-selector":"z"},{"value":863198232182784},{"value":0},{"value":863198232182784},{"value":2},{"value":4294967295},{"value":0},{"value":17179869184},{"value":0},{"value":2},{"value":0},{"value":0},{"value":200979},{"value":0},{"value":18446744073709551569},{"value":8329287384},{"value":0},{"value":4294967295},{"value":2},{"value":863198232182784},{"value":0},{"value":863198232182784},{"value":6408756952},{"value":8589934592,"objc-selector":"z"},{"value":21592279046},{"value":18446744073709550527},{"value":4412409862}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491435048},"cpsr":{"value":0},"fp":{"value":6408756800},"sp":{"value":6408756720},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491360308},"far":{"value":0}},"queue":"com.apple.root.user-interactive-qos","frames":[{"imageOffset":3124,"symbol":"mach_msg2_trap","symbolLocation":8,"imageIndex":12},{"imageOffset":77864,"symbol":"mach_msg2_internal","symbolLocation":76,"imageIndex":12},{"imageOffset":39308,"symbol":"mach_msg_overwrite","symbolLocation":484,"imageIndex":12},{"imageOffset":4020,"symbol":"mach_msg","symbolLocation":24,"imageIndex":12},{"imageOffset":392112,"symbol":"__CFRunLoopServiceMachPort","symbolLocation":160,"imageIndex":6},{"imageOffset":386312,"symbol":"__CFRunLoopRun","symbolLocation":1188,"imageIndex":6},{"imageOffset":1150780,"symbol":"_CFRunLoopRunSpecificWithOptions","symbolLocation":532,"imageIndex":6},{"imageOffset":10861840,"symbol":"-[NSRunLoop(NSRunLoop) runMode:beforeDate:]","symbolLocation":212,"imageIndex":21},{"imageOffset":5689540,"symbol":"-[NSAnimation _runBlocking]","symbolLocation":412,"imageIndex":8},{"imageOffset":7004,"symbol":"_dispatch_call_block_and_release","symbolLocation":32,"imageIndex":22},{"imageOffset":113364,"symbol":"_dispatch_client_callout","symbolLocation":16,"imageIndex":22},{"imageOffset":231900,"symbol":"<deduplicated_symbol>","symbolLocation":32,"imageIndex":22},{"imageOffset":82236,"symbol":"_dispatch_root_queue_drain","symbolLocation":736,"imageIndex":22},{"imageOffset":83844,"symbol":"_dispatch_worker_thread2","symbolLocation":180,"imageIndex":22},{"imageOffset":11792,"symbol":"_pthread_wqthread","symbolLocation":232,"imageIndex":13},{"imageOffset":7068,"symbol":"start_wqthread","symbolLocation":8,"imageIndex":13}]},{"id":5531597,"frames":[],"threadState":{"x":[{"value":6416019456},{"value":0},{"value":6415482880},{"value":0},{"value":278532},{"value":18446744073709551615},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":0},"cpsr":{"value":0},"fp":{"value":0},"sp":{"value":6416019456},"esr":{"value":0},"pc":{"value":6491614100},"far":{"value":0}}},{"id":5532249,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6413348360},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037063464},{"value":54037063528},{"value":6413349088},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6413348480},"sp":{"value":6413348336},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532250,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6413921800},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037060392},{"value":54037060456},{"value":6413922528},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6413921920},"sp":{"value":6413921776},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532251,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":500000000},{"value":6414495240},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037056808},{"value":54037056872},{"value":6414495968},{"value":500000000},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6414495360},"sp":{"value":6414495216},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532252,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998958},{"value":6415068680},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037060904},{"value":54037060968},{"value":6415069408},{"value":499998958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6415068800},"sp":{"value":6415068656},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532253,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998958},{"value":6418427400},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037056296},{"value":54037056360},{"value":6418428128},{"value":499998958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6418427520},"sp":{"value":6418427376},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532254,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6419656200},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037062440},{"value":54037062504},{"value":6419656928},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6419656320},"sp":{"value":6419656176},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532255,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6442429960},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037069096},{"value":54037069160},{"value":6442430688},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6442430080},"sp":{"value":6442429936},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532256,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":14481108488},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037066536},{"value":54037066600},{"value":14481109216},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":14481108608},"sp":{"value":14481108464},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532257,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999000},{"value":14481681928},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037058856},{"value":54037058920},{"value":14481682656},{"value":499999000},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":14481682048},"sp":{"value":14481681904},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532258,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":14482255368},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037062952},{"value":54037063016},{"value":14482256096},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":14482255488},"sp":{"value":14482255344},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532259,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998958},{"value":14482828808},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037054760},{"value":54037054824},{"value":14482829536},{"value":499998958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":14482828928},"sp":{"value":14482828784},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532260,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":14483402248},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54062181672},{"value":54062181736},{"value":14483402976},{"value":499999959},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":14483402368},"sp":{"value":14483402224},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532261,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":14483975688},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54062171432},{"value":54062171496},{"value":14483976416},{"value":499999959},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":14483975808},"sp":{"value":14483975664},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532262,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":14484549128},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54062176552},{"value":54062176616},{"value":14484549856},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":14484549248},"sp":{"value":14484549104},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532266,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":500000000},{"value":6099660296},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54062176040},{"value":54062176104},{"value":6099661024},{"value":500000000},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6099660416},"sp":{"value":6099660272},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532267,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":6132346376},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54062170408},{"value":54062170472},{"value":6132347104},{"value":499999959},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6132346496},"sp":{"value":6132346352},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532268,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499998917},{"value":6133493256},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037058344},{"value":54037058408},{"value":6133493984},{"value":499998917},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6133493376},"sp":{"value":6133493232},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532269,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6134066696},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037068584},{"value":54037068648},{"value":6134067424},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6134066816},"sp":{"value":6134066672},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532270,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":6134640136},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037065512},{"value":54037065576},{"value":6134640864},{"value":499999959},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6134640256},"sp":{"value":6134640112},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532271,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6135213576},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037067048},{"value":54037067112},{"value":6135214304},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6135213696},"sp":{"value":6135213552},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532272,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6135787016},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037067560},{"value":54037067624},{"value":6135787744},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6135787136},"sp":{"value":6135786992},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532273,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6136360456},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54037068072},{"value":54037068136},{"value":6136361184},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6136360576},"sp":{"value":6136360432},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532274,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6136933896},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54047576360},{"value":54047576424},{"value":6136934624},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6136934016},"sp":{"value":6136933872},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532275,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":6137507336},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54054466856},{"value":54054466920},{"value":6137508064},{"value":499999959},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6137507456},"sp":{"value":6137507312},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532276,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6138080776},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54054460712},{"value":54054460776},{"value":6138081504},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6138080896},"sp":{"value":6138080752},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532277,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":6138654216},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54054456616},{"value":54054456680},{"value":6138654944},{"value":499999959},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6138654336},"sp":{"value":6138654192},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532278,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999959},{"value":6139227656},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54054465320},{"value":54054465384},{"value":6139228384},{"value":499999959},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6139227776},"sp":{"value":6139227632},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532279,"name":"Pool","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":160},{"value":0},{"value":499999958},{"value":6139801096},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54054460200},{"value":54054460264},{"value":6139801824},{"value":499999958},{"value":0},{"value":0},{"value":1},{"value":256},{"value":0},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635932},"cpsr":{"value":1610612736},"fp":{"value":6139801216},"sp":{"value":6139801072},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28892,"symbol":"_pthread_cond_wait","symbolLocation":984,"imageIndex":13},{"imageOffset":141292,"symbol":"std::__1::condition_variable::__do_timed_wait(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::system_clock, std::__1::chrono::duration<long long, std::__1::ratio<1l, 1000000000l>>>)","symbolLocation":104,"imageIndex":17},{"imageOffset":8825276,"imageIndex":1},{"imageOffset":8605052,"imageIndex":1},{"imageOffset":8774144,"imageIndex":1},{"imageOffset":8607548,"imageIndex":1},{"imageOffset":8798532,"imageIndex":1},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532287,"name":"CVDisplayLink","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":65704},{"value":0},{"value":6354250},{"value":2305},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54053604408},{"value":54053604472},{"value":1},{"value":6354250},{"value":0},{"value":0},{"value":2305},{"value":2560},{"value":4604585904495},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635976},"cpsr":{"value":2684354560},"fp":{"value":6140374448},"sp":{"value":6140374304},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28936,"symbol":"_pthread_cond_wait","symbolLocation":1028,"imageIndex":13},{"imageOffset":15164,"symbol":"CVDisplayLink::waitUntil(unsigned long long)","symbolLocation":336,"imageIndex":23},{"imageOffset":11300,"symbol":"CVDisplayLink::runIOThread()","symbolLocation":500,"imageIndex":23},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532289,"name":"CVDisplayLink","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":65704},{"value":0},{"value":6814959},{"value":2049},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54053609016},{"value":54053609080},{"value":1},{"value":6814959},{"value":0},{"value":0},{"value":2049},{"value":2304},{"value":4604585904495},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635976},"cpsr":{"value":2684354560},"fp":{"value":6409907632},"sp":{"value":6409907488},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28936,"symbol":"_pthread_cond_wait","symbolLocation":1028,"imageIndex":13},{"imageOffset":15164,"symbol":"CVDisplayLink::waitUntil(unsigned long long)","symbolLocation":336,"imageIndex":23},{"imageOffset":11300,"symbol":"CVDisplayLink::runIOThread()","symbolLocation":500,"imageIndex":23},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532294,"name":"CVDisplayLink","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":65704},{"value":0},{"value":6173500},{"value":2049},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54053601336},{"value":54053601400},{"value":1},{"value":6173500},{"value":0},{"value":0},{"value":2049},{"value":2304},{"value":4604585904495},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635976},"cpsr":{"value":2684354560},"fp":{"value":6140947888},"sp":{"value":6140947744},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28936,"symbol":"_pthread_cond_wait","symbolLocation":1028,"imageIndex":13},{"imageOffset":15164,"symbol":"CVDisplayLink::waitUntil(unsigned long long)","symbolLocation":336,"imageIndex":23},{"imageOffset":11300,"symbol":"CVDisplayLink::runIOThread()","symbolLocation":500,"imageIndex":23},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]},{"id":5532295,"name":"CVDisplayLink","threadState":{"x":[{"value":260},{"value":0},{"value":0},{"value":0},{"value":0},{"value":65704},{"value":0},{"value":6331917},{"value":2049},{"value":0},{"value":0},{"value":2},{"value":2},{"value":0},{"value":0},{"value":0},{"value":305},{"value":8329285496},{"value":0},{"value":54053615160},{"value":54053615224},{"value":1},{"value":6331917},{"value":0},{"value":0},{"value":2049},{"value":2304},{"value":4604585904495},{"value":0}],"flavor":"ARM_THREAD_STATE64","lr":{"value":6491635976},"cpsr":{"value":2684354560},"fp":{"value":6409334192},"sp":{"value":6409334048},"esr":{"value":1442840704,"description":"(Syscall)"},"pc":{"value":6491374840},"far":{"value":0}},"frames":[{"imageOffset":17656,"symbol":"__psynch_cvwait","symbolLocation":8,"imageIndex":12},{"imageOffset":28936,"symbol":"_pthread_cond_wait","symbolLocation":1028,"imageIndex":13},{"imageOffset":15164,"symbol":"CVDisplayLink::waitUntil(unsigned long long)","symbolLocation":336,"imageIndex":23},{"imageOffset":11300,"symbol":"CVDisplayLink::runIOThread()","symbolLocation":500,"imageIndex":23},{"imageOffset":27656,"symbol":"_pthread_start","symbolLocation":136,"imageIndex":13},{"imageOffset":7080,"symbol":"thread_start","symbolLocation":8,"imageIndex":13}]}],
  "usedImages" : [
  {
    "source" : "P",
    "arch" : "arm64",
    "base" : 4368351232,
    "CFBundleShortVersionString" : "1.0.0",
    "CFBundleIdentifier" : "com.yourcompany.Pitchenga",
    "size" : 7454720,
    "uuid" : "d37f7bd1-e877-35a4-b254-26c769c0234e",
    "path" : "\/Users\/USER\/*\/Pitchenga.app\/Contents\/MacOS\/Pitchenga",
    "name" : "Pitchenga",
    "CFBundleVersion" : "1.0.0"
  },
  {
    "source" : "P",
    "arch" : "arm64",
    "base" : 4574412800,
    "CFBundleShortVersionString" : "1.7.0-c2c6d65",
    "CFBundleIdentifier" : "com.splice.Instrument",
    "size" : 23117824,
    "uuid" : "16497075-975a-3e52-b670-57e5ab7d3ace",
    "path" : "\/Library\/Audio\/Plug-Ins\/Components\/Splice INSTRUMENT.component\/Contents\/MacOS\/Splice INSTRUMENT",
    "name" : "Splice INSTRUMENT",
    "CFBundleVersion" : "1.7.0"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 4527702016,
    "CFBundleShortVersionString" : "1.14",
    "CFBundleIdentifier" : "com.apple.audio.units.Components",
    "size" : 1327104,
    "uuid" : "d4431c82-73db-32cd-aa69-fb5df573e77e",
    "path" : "\/System\/Library\/Components\/CoreAudio.component\/Contents\/MacOS\/CoreAudio",
    "name" : "CoreAudio",
    "CFBundleVersion" : "1.14"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 4526981120,
    "size" : 49152,
    "uuid" : "f26af954-d1f7-31aa-9981-1bad216149d9",
    "path" : "\/usr\/lib\/libobjc-trampolines.dylib",
    "name" : "libobjc-trampolines.dylib"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 4867997696,
    "CFBundleShortVersionString" : "345.20.4",
    "CFBundleIdentifier" : "com.apple.AGXMetalG16X",
    "size" : 8617984,
    "uuid" : "f3801a6f-cfb2-3ee3-a8a0-320185aa69b6",
    "path" : "\/System\/Library\/Extensions\/AGXMetalG16X.bundle\/Contents\/MacOS\/AGXMetalG16X",
    "name" : "AGXMetalG16X",
    "CFBundleVersion" : "345.20.4"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 4563304448,
    "CFBundleShortVersionString" : "1.0",
    "CFBundleIdentifier" : "com.apple.AppleMetalOpenGLRenderer",
    "size" : 409600,
    "uuid" : "66a35ba7-f2f4-3a44-9a0c-8440564e9578",
    "path" : "\/System\/Library\/Extensions\/AppleMetalOpenGLRenderer.bundle\/Contents\/MacOS\/AppleMetalOpenGLRenderer",
    "name" : "AppleMetalOpenGLRenderer",
    "CFBundleVersion" : "1"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6491893760,
    "CFBundleShortVersionString" : "6.9",
    "CFBundleIdentifier" : "com.apple.CoreFoundation",
    "size" : 5542592,
    "uuid" : "646518bb-a6e8-3da7-ab32-9d97bcbdc25d",
    "path" : "\/System\/Library\/Frameworks\/CoreFoundation.framework\/Versions\/A\/CoreFoundation",
    "name" : "CoreFoundation",
    "CFBundleVersion" : "4302"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6704787456,
    "CFBundleShortVersionString" : "2.1.1",
    "CFBundleIdentifier" : "com.apple.HIToolbox",
    "size" : 3158656,
    "uuid" : "3c068ca7-e6a9-3e91-953a-b527a1892d05",
    "path" : "\/System\/Library\/Frameworks\/Carbon.framework\/Versions\/A\/Frameworks\/HIToolbox.framework\/Versions\/A\/HIToolbox",
    "name" : "HIToolbox"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6563561472,
    "CFBundleShortVersionString" : "6.9",
    "CFBundleIdentifier" : "com.apple.AppKit",
    "size" : 24299264,
    "uuid" : "bf94fd49-283d-3c62-8cf0-0a49c21b6129",
    "path" : "\/System\/Library\/Frameworks\/AppKit.framework\/Versions\/C\/AppKit",
    "name" : "AppKit",
    "CFBundleVersion" : "2685.40.108"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6487625728,
    "size" : 653076,
    "uuid" : "044cd67e-3a0a-3ca4-8bb3-a9687d5328fe",
    "path" : "\/usr\/lib\/dyld",
    "name" : "dyld"
  },
  {
    "size" : 0,
    "source" : "A",
    "base" : 0,
    "uuid" : "00000000-0000-0000-0000-000000000000"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6487080960,
    "size" : 341180,
    "uuid" : "9e66cff2-3ebd-3242-8166-d5d0c204755b",
    "path" : "\/usr\/lib\/libobjc.A.dylib",
    "name" : "libobjc.A.dylib"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6491357184,
    "size" : 246944,
    "uuid" : "78ec33a6-6330-3836-8900-eb90836936e8",
    "path" : "\/usr\/lib\/system\/libsystem_kernel.dylib",
    "name" : "libsystem_kernel.dylib"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6491607040,
    "size" : 51916,
    "uuid" : "0596a7b6-bce2-3f06-a2e8-3eaab5371ed8",
    "path" : "\/usr\/lib\/system\/libsystem_pthread.dylib",
    "name" : "libsystem_pthread.dylib"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 7010803712,
    "CFBundleShortVersionString" : "2.0",
    "CFBundleIdentifier" : "com.apple.audio.midi.CoreMIDI",
    "size" : 840864,
    "uuid" : "349e1842-7521-35a6-aca9-e80329ec86a8",
    "path" : "\/System\/Library\/Frameworks\/CoreMIDI.framework\/Versions\/A\/CoreMIDI",
    "name" : "CoreMIDI",
    "CFBundleVersion" : "88"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6699839488,
    "CFBundleShortVersionString" : "1.0",
    "CFBundleIdentifier" : "com.apple.audio.caulk",
    "size" : 167296,
    "uuid" : "67d6bcc2-f024-388e-b546-b1c2fe6f0ddc",
    "path" : "\/System\/Library\/PrivateFrameworks\/caulk.framework\/Versions\/A\/caulk",
    "name" : "caulk"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6543081472,
    "CFBundleShortVersionString" : "5.0",
    "CFBundleIdentifier" : "com.apple.audio.CoreAudio",
    "size" : 8065952,
    "uuid" : "16b7552a-ead3-374c-a642-5a11a7523189",
    "path" : "\/System\/Library\/Frameworks\/CoreAudio.framework\/Versions\/A\/CoreAudio",
    "name" : "CoreAudio",
    "CFBundleVersion" : "5.0"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6490644480,
    "size" : 601764,
    "uuid" : "652836ca-32b1-3388-a72a-d6b90ddda958",
    "path" : "\/usr\/lib\/libc++.1.dylib",
    "name" : "libc++.1.dylib"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6488784896,
    "size" : 707792,
    "uuid" : "a6fb5220-05f1-36a3-93ca-e45f15c98d3a",
    "path" : "\/usr\/lib\/system\/libcorecrypto.dylib",
    "name" : "libcorecrypto.dylib"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 7052689408,
    "CFBundleShortVersionString" : "21623",
    "CFBundleIdentifier" : "com.apple.JavaScriptCore",
    "size" : 27468448,
    "uuid" : "fe18d222-b3fa-39cb-a89b-1001fa2d1878",
    "path" : "\/System\/Library\/Frameworks\/JavaScriptCore.framework\/Versions\/A\/JavaScriptCore",
    "name" : "JavaScriptCore",
    "CFBundleVersion" : "21623.2.7.11.7"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 7189516288,
    "CFBundleShortVersionString" : "21623",
    "CFBundleIdentifier" : "com.apple.WebKit",
    "size" : 23341920,
    "uuid" : "c13188b8-0146-3f71-8e33-8f6808e4218a",
    "path" : "\/System\/Library\/Frameworks\/WebKit.framework\/Versions\/A\/WebKit",
    "name" : "WebKit",
    "CFBundleVersion" : "21623.2.7.11.7"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6517399552,
    "CFBundleShortVersionString" : "6.9",
    "CFBundleIdentifier" : "com.apple.Foundation",
    "size" : 16399392,
    "uuid" : "187e7913-b154-30a7-8070-852767aac3cf",
    "path" : "\/System\/Library\/Frameworks\/Foundation.framework\/Versions\/C\/Foundation",
    "name" : "Foundation",
    "CFBundleVersion" : "4302"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6489804800,
    "size" : 290400,
    "uuid" : "4c58ab31-f363-3e75-a8f8-302105812dbf",
    "path" : "\/usr\/lib\/system\/libdispatch.dylib",
    "name" : "libdispatch.dylib"
  },
  {
    "source" : "P",
    "arch" : "arm64e",
    "base" : 6666350592,
    "CFBundleShortVersionString" : "1.8",
    "CFBundleIdentifier" : "com.apple.CoreVideo",
    "size" : 540704,
    "uuid" : "67098f59-d15b-3748-bcb9-c8041ce75424",
    "path" : "\/System\/Library\/Frameworks\/CoreVideo.framework\/Versions\/A\/CoreVideo",
    "name" : "CoreVideo",
    "CFBundleVersion" : "726.2"
  }
],
  "sharedCache" : {
  "base" : 6486540288,
  "size" : 5820792832,
  "uuid" : "674db25a-34b2-3c56-8bd4-7d78005b2f2e"
},
  "vmSummary" : "ReadOnly portion of Libraries: Total=1.8G resident=0K(0%) swapped_out_or_unallocated=1.8G(100%)\nWritable regions: Total=5.0G written=2259K(0%) resident=2259K(0%) swapped_out=0K(0%) unallocated=5.0G(100%)\n\n                                VIRTUAL   REGION \nREGION TYPE                        SIZE    COUNT (non-coalesced) \n===========                     =======  ======= \n.note.gnu.proper                    320        1 \nAccelerate framework               256K        2 \nActivity Tracing                   256K        1 \nAttributeGraph Data               1024K        1 \nCG image                           432K        6 \nColorSync                           64K        4 \nCoreAnimation                     12.6M       32 \nCoreGraphics                       112K        7 \nCoreUI image data                  320K        5 \nFoundation                          16K        1 \nKernel Alloc Once                   32K        1 \nMALLOC                           666.8M      154 \nMALLOC guard page                 3328K        4 \nMALLOC_LARGE (reserved)           8192K        1         reserved VM address space (unallocated)\nSTACK GUARD                       57.6M      103 \nStack                             62.2M      103 \nVM_ALLOCATE                       1056K       26 \nVM_ALLOCATE (reserved)             4.0G        2         reserved VM address space (unallocated)\nWebKit Malloc                    256.1M        8 \n__AUTH                            5873K      649 \n__AUTH_CONST                      88.5M     1027 \n__CTF                               824        1 \n__DATA                            35.7M      984 \n__DATA_CONST                      33.7M     1038 \n__DATA_DIRTY                      8428K      885 \n__FONT_DATA                        2352        1 \n__GLSLBUILTINS                    5176K        1 \n__INFO_FILTER                         8        1 \n__LINKEDIT                       593.8M        7 \n__OBJC_RO                         78.4M        1 \n__OBJC_RW                         2571K        1 \n__TEXT                             1.3G     1060 \n__TPRO_CONST                       128K        2 \nmapped file                        2.8G       65 \npage table in kernel              2259K        1 \nshared memory                     1568K       20 \n===========                     =======  ======= \nTOTAL                             10.0G     6206 \nTOTAL, minus reserved VM space     6.0G     6206 \n",
  "legacyInfo" : {
  "threadTriggered" : {
    "name" : "JUCE v8.0.12: Message Thread",
    "queue" : "com.apple.main-thread"
  }
},
  "logWritingSignature" : "05a167465fdc6ab70b3b3556063539ebaa922bd3",
  "roots_installed" : 0,
  "bug_type" : "309",
  "trmStatus" : 8192,
  "trialInfo" : {
  "rollouts" : [
    {
      "rolloutId" : "695fd05d8ca5554688521e5e",
      "factorPackIds" : [
        "695fd08781fcd20ded79c1d3",
        "695fd0d28ca5554688521e5f",
        "695fd09c8774dc09015a80e9",
        "695fd0b18774dc09015a80ea"
      ],
      "deploymentId" : 3
    },
    {
      "rolloutId" : "648cada15dbc71671bb3aa1b",
      "factorPackIds" : [
        "65a81173096f6a1f1ba46525"
      ],
      "deploymentId" : 240000116
    }
  ],
  "experiments" : [

  ]
}
}

