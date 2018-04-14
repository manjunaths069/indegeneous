import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;


public class ServerProgram 
{
	public static void main(String args[])
	{
		try
		{
			ServerSocket ss = new ServerSocket(2222);
			
			Socket s = ss.accept();
			
			InputStream is = s.getInputStream();
			BufferedReader br = new BufferedReader(new InputStreamReader(is));
			String temp = "";
			while((temp = br.readLine())!=null)
			{
				System.out.println(""+temp);
			}
		}
		catch(IOException ioe)
		{
			ioe.printStackTrace();
		}
	}
	

}
