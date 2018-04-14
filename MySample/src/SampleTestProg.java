import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;


public class SampleTestProg {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		String str = "MY NAME";
		HashMap<String,Integer> map = new HashMap<String,Integer>();
		Integer count = 0;
		
		String s,s1;
		while(str.length() > 0){
			count = 0;
			String singleChar = str.substring(0,1);
			str = str.substring(1);
			
			if((count = map.put(singleChar, 1))!= null){
				map.put(singleChar, count + 1);
			}
			
			 s = "Naveen";
			 s1 = new String("Naveen");
			
			
			/*if (null == map.get(singleChar)) {
				map.put(singleChar, 1);
			} else {
				count = map.get(singleChar).intValue();
				map.put(singleChar, count + 1);
			}*/
			
		}
		
		Set<String> keys = map.keySet();
		Iterator<String> itr = keys.iterator();
		while(itr.hasNext()){
			String key = itr.next();
			System.out.println("Key:"+key+"   count:"+map.get(key));
		}
		

	}

}
