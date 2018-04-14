
public class ReverseString {
	
	static char[] ReverseAllWords(char[] in_text)
    {
        int lindex = 0;
        int rindex = in_text.length - 1;
        if (rindex > 1)
        {
            //reverse complete phrase
            in_text = ReverseString(in_text, 0, rindex);

            //reverse each word in resultant reversed phrase
            for (rindex = 0; rindex <= in_text.length; rindex++)
            {
                if (rindex == in_text.length || in_text[rindex] == ' ')
                {
                    in_text = ReverseString(in_text, lindex, rindex - 1);
                    lindex = rindex + 1;
                }
            }
        }
        return in_text;
    }

    static char[] ReverseString(char[] intext, int lindex, int rindex)
    {
        char tempc;
        while (lindex < rindex)
        {
            tempc = intext[lindex];
            intext[lindex++] = intext[rindex];
            intext[rindex--] = tempc;
        }
        return intext;
    }
    
    public static void main(String args[])
    {
    	String str = "Hi I am Naveen";
    	char arr[] = str.toCharArray(); 
    	char arr1[]=ReverseAllWords(arr);
    	System.out.println("Reversed string is ");
    	for(int i=0;i<arr.length;i++)
    		System.out.print(arr1[i]);
    	System.out.println();
    	
    	try{}
    	catch(Exception e)
    	{
    		e.printStackTrace();
    	}
    	catch(ArrayIndexOutOfBoundsException e1)
    	{
    		e1.printStackTrace();
    	}
    }

}
