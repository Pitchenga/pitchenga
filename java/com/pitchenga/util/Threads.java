package com.pitchenga.util;

import java.util.concurrent.ThreadFactory;
import java.util.concurrent.atomic.AtomicLong;

public class Threads implements ThreadFactory {

    public static final AtomicLong ID_COUNTER = new AtomicLong(-1);
    private final String prefix;

    public Threads(String prefix) {
        this.prefix = prefix;
    }

    @Override
    public Thread newThread(Runnable runnable) {
        Runnable myRunnable = () -> {
            System.out.println("### " + Thread.currentThread().getName() + " started");
            try {
                runnable.run();
            } catch (Throwable e) {
                System.out.println("### " + Thread.currentThread().getName() + " threw " + e);
                e.printStackTrace();
                throw e;
            } finally {
                System.out.println("### " + Thread.currentThread().getName() + " finished");
            }
        };
        Thread thread = new Thread(myRunnable);
        thread.setName(prefix + ID_COUNTER.incrementAndGet());
        return thread;
    }
}
