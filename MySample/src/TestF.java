
import java.io.*;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;

public class TestF {
	
	public static void main(String args[])
	{
		
		try {
			// Get a file channel for the file
			File file = new File(
					"D:\\temp\\temp\\CksmTools\\Adapter-DefaultsChecksum80.txt");
			RandomAccessFile ra = new RandomAccessFile(file, "rw");
			 FileChannel channel = ra.getChannel();
			 //
			// Use the file channel to create a lock on the file.
			// This method blocks until it can retrieve the lock.
			

			// Try acquiring the lock without blocking. This method returns
			// null or throws an exception if the file is already locked.
			try {
				FileLock lock = channel.lock();
				//lock = channel.tryLock();
				Thread.currentThread().sleep(600000);
			} catch (Exception e) {
				e.printStackTrace();
			}

			// Release the lock
			// lock.release();

			// Close the file
			channel.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
	}

}
