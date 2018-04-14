

import java.sql.CallableStatement;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;

public class DBTest {

  Connection m_Connection = null;
  CallableStatement m_Statement = null;
  ResultSet m_ResultSet = null;
  
  private String mMachine ="v3-w23-rds2056";
  private String mPortNo ="2638";
  //private String mSid ="";
  private String mUserid = "dba";
  private String mPassword = "pronet";
  
  
  String m_Driver = "com.sybase.jdbc3.jdbc.SybDriver";
  //String m_Url = "jdbc:microsoft:sqlserver://localhost:1433;DatabaseName=MyDatabase";
  String m_Url= "jdbc:sybase:Tds:" + mMachine + ":" + mPortNo;// + "?ServiceName=" + mSid;
  		 //mURL = "jdbc:sybase:Tds:localhost:2638";
  public DBTest() {

    //Load driver
    try {
      Class.forName(m_Driver);
    }
    catch (ClassNotFoundException ex) {
      ex.printStackTrace();
    }
  }

  public void doWork(int sP_type ) {

    String query = "";
    
    try {
      //Create connection object
    	
      m_Connection = DriverManager.getConnection(m_Url, mUserid, mPassword);
      //System.out.println(m_Connection+" Open");
      //System.out.println("testing 1");
      //Create Statement object
      if(sP_type == 1)
      {
    	  
    	//System.out.println("This is first stored procedure (getClusterNames)");
        m_Statement = m_Connection.prepareCall("exec getClusterNames ?");
        m_Statement.setString(1, "vmvc-blr-01.bmc.com_vCenter"); 
      	
      }
      else if (sP_type == 2)
      {
        	//System.out.println("This is second stored procedure (getHostNames)");
            m_Statement = m_Connection.prepareCall("exec getHostNames ? ?");
            m_Statement.setString(1, "vmvc-blr-01.bmc.com_vCenter"); 
            m_Statement.setString(2, "SRI-CLUSTER-New");

      }
      else if (sP_type == 3){

    	  	//System.out.println("This is third stored procedure (getDataStoreNames)");
			m_Statement = m_Connection.prepareCall("exec getDataStoreNames ? ?");
			m_Statement.setString(1, "vmvc-blr-01.bmc.com_vCenter"); 
			m_Statement.setString(2, "esx-dev-blr2.bmc.com");

			//or m_Statement.setString(1, "vmvc-blr-01.bmc.com_vCenter"); 
			//m_Statement.setString(2, "esx-dev-blr3.bmc.com");

      }
      else{
    	
    	  System.out.println("This is invalid option (getDataStoreNames)");
    	  return;
      }
      	
      
      
      m_ResultSet= m_Statement.executeQuery();
      
      while(m_ResultSet.next()){
    	  System.out.println(m_ResultSet.getString(1));
      }
      
      
      //System.out.println("testing 2");
//      //query = "SELECT * FROM MyTable";
//      query = "select * from group_tree_view";
//      //Execute the query
//      m_ResultSet = m_Statement.executeQuery(query);
//      //m_Statement.
//
//      //Loop through the results
//      while (m_ResultSet.next()) {
//
//          System.out.print(m_ResultSet.getString(1));
//          System.out.print(", ");
//          System.out.print(m_ResultSet.getString(2));
//          System.out.print(", \n");
//          //System.out.print(m_ResultSet.getString(3));
//          //System.out.print("\n"); //new line
//          
//      }
   }
    catch (SQLException ex) {
      ex.printStackTrace();
      System.out.println(query);
    }
    finally {

      try {
        if (m_ResultSet != null)
          m_ResultSet.close();
        if (m_Statement != null)
          m_Statement.close();
        if (m_Connection != null){
          m_Connection.close();
          //System.out.println(m_Connection+" Closed");
        }
      }
      catch (SQLException ex) {
        ex.printStackTrace();
      }
    }
  }

  public static void main(String[] args) {
    DBTest dbTest = new DBTest();
    dbTest.doWork(1);
  }
}