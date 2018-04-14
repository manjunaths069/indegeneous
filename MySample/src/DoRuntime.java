
import java.io.*;
import java.util.*;


public class DoRuntime
{


public static void main(String args[])  throws IOException
{

	if(args.length <= 0)
	{

		System.err.println("Need Command to run");
		System.exit(-1);
	}

	Runtime rt = Runtime.getRuntime();
	Process ps = rt.exec(args);
	InputStream is = ps.getInputStream();
	InputStreamReader isr = new InputStreamReader(is);
	BufferedReader br = new BufferedReader(isr);

	String line;

	System.out.printf("Output of running %s is :",Arrays.toString(args));

	while((line =  br.readLine()) != null)
	{
		System.out.println(line);

	}
}

}
