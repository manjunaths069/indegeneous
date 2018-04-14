import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.net.InetAddress;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;



public class DeleteDeviceAndAssociatedInstances {

	static PrintStream orgStream = null;
	static File console_output = null;
	static PrintStream consoleFileStream = null;
	
	static{
		orgStream = System.out;

		console_output = new File("DeviceCleanup.log");
		try {
			consoleFileStream = new PrintStream(new FileOutputStream(
					console_output, true));
		}catch(FileNotFoundException fe){
			fe.printStackTrace();
			System.exit(1);
		}
	}
	public static void main(String args[]) throws Exception
	{		 
		if (args != null && args.length > 0) {
			 String strServerPath = null, deviceName=null, hostIdstr=null;
			
			for(int i = 0; i < args.length; i++)
			{
				String arg = args[i];
				if( arg.toLowerCase().equals("-serverpath"))	strServerPath = args[++i];
				else if(arg.toLowerCase().equals("-devicename")) deviceName = args[++i];
				else if(arg.toLowerCase().equals("-hostid")) hostIdstr = args[++i];
				//else usage();
			}
			
			if( strServerPath == null || strServerPath.trim().equals("") || !strServerPath.endsWith("pw"))
			{
				System.out.println("ERROR. Please specify the server path. (pw directory). Missing required param : server path provided is :"+strServerPath);
				System.exit(1);
			}

			if( (deviceName == null || deviceName.trim().equals("")) &&  (hostIdstr == null || hostIdstr.trim().equals("")))
			{
				System.out.println("ERROR. Please specify the device name or host id that needs to be deleted. Missing required param");			
				System.exit(1);
			}
			
			Connection con = null;

			PreparedStatement findHostIdPStmt = null;
			PreparedStatement findMcUdidPStmt= null;
			PreparedStatement findInstanceOfDeviceQuerypstmt = null;			
			ResultSet rs = null;
			try {

				con = DBConnection.getInstance().getConnection();
				
				if (con != null) {
					con.setAutoCommit(false);
					console("\nProcessing......\n");
					log(" Start Deleting.........\n");
					String findHostIDdQuery = "select hostid, mcudid from device_cntl where username = ?";
					String findMCudidiQuery = "select mcudid from device_cntl where hostid = ?";
					String findInstanceOfDeviceQuery = "select itemid from item_cfg where hostid = ?";					
					findHostIdPStmt = con.prepareStatement(findHostIDdQuery);
					findMcUdidPStmt = con.prepareStatement(findMCudidiQuery);
					findInstanceOfDeviceQuerypstmt = con.prepareStatement(findInstanceOfDeviceQuery);					

					boolean isValidDelete = true;
					int hostId=0;
					String mcUdid=null;
					if(hostIdstr != null && !hostIdstr.equals("")){
						findMcUdidPStmt.clearParameters();
						int id=0;
						try{
							id=Integer.parseInt(hostIdstr);
						}catch(NumberFormatException nfe){
							console("NFE.Device not found with hostid " + hostIdstr +". Please provide valid host id");
						}
						findMcUdidPStmt.setInt(1, id);
						rs = findMcUdidPStmt.executeQuery();
						while (rs.next()) {							
							mcUdid=rs.getString("mcudid");	
						}
						hostId=id;
						
					}else{
						findHostIdPStmt.clearParameters();					
						findHostIdPStmt.setString(1, deviceName);
						rs = findHostIdPStmt.executeQuery();
						int count=0;
						while (rs.next()) {
							hostId=rs.getInt("hostid");
							mcUdid=rs.getString("mcudid");							
							count++;
							if(count > 1){
								log("Found duplicate devices for " + deviceName + "  Not sure which device to be deleted");
								console("Found duplicate devices for " + deviceName + "  Not sure which device to be deleted");
								break;
							}
						}						
						
						if(count > 1){							
							isValidDelete=false;
						}
					}
					
						if(hostId == 0 || mcUdid == null){
							log("Device not found " + deviceName+". Host Id value is : "+hostId+" And MCUDID value is "+mcUdid);
							console("Device not found " + deviceName+". Host Id value is : "+hostId+" And MCUDID value is "+mcUdid);
							isValidDelete=false;
						}						
						rs.close();
						rs = null;
						
						
						if(isValidDelete){
							ArrayList<Integer> instList=null;
							findInstanceOfDeviceQuerypstmt.clearParameters();
							findInstanceOfDeviceQuerypstmt.setInt(1, hostId);
							rs = findInstanceOfDeviceQuerypstmt.executeQuery();
							
							if(rs != null){
								instList=new ArrayList<Integer>();
								while (rs.next()) {
									instList.add(rs.getInt("itemid"));
								}
								rs.close();
								rs = null;
							}else{
								log("Result set is null. No instances found in device " + deviceName + "Host id : "+hostId); 
							}
							if(instList == null || instList.size()==0){
							   log("No instances found in device " + deviceName+ "Host id : "+hostId);
							   log("Deleting the device " + deviceName + "Host id : "+hostId);
							   deleteDevice(con, hostId, mcUdid, strServerPath);						   
							}else{
								log("Deleting the instances available in device " + deviceName + ": Host id : "+hostId);
								deleteInstances(con, instList);
								log("Deleting the device " + deviceName + ": Host id : "+hostId);
								deleteDevice(con, hostId, mcUdid, strServerPath);								
							}
							console("Device deleted successfully");
						}						
						con.commit();
					log(" Execution completed.\n");
					console("Log file is available at "+console_output.getAbsolutePath());
				} else {
					console("DB connection not available");
				}

			} catch (Exception e) {
				console("Excption: ");
				e.printStackTrace();
			} finally {

				try {
					if (rs != null) {
						rs.close();
					}
					if (findHostIdPStmt != null) {
						findHostIdPStmt.close();
					}
					if (findMcUdidPStmt != null) {
						findMcUdidPStmt.close();
					}
					if (con != null) {
						con.close();
					}
					if(consoleFileStream != null)
					consoleFileStream.close();
				} catch (Exception e) {
					console("Excption:");
					e.printStackTrace();
				}

			}

		} else {
			System.out.println("Please provide the device name to be deleted.");
		}

	}
	
	
	private static String getDateTime() {
		DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
		Date date = new Date();
		return dateFormat.format(date);
	}

	private static void log(String msg) {
		System.setOut(consoleFileStream);
		System.out.println(getDateTime() + " - " + msg);
		System.out.println();
	}
	
	private static void console(String msg) {
		System.setOut(orgStream);
		System.out.println(msg);
		System.out.println();
	}
	
	private static void deleteDevice(Connection conn, int hostId, String mcudid, String serverPath)
	{
		if(conn == null || hostId == 0 || mcudid == null){ 
			log("Not able to delete the device. Connection is null or host id is 0. Connection: "+conn +" Host id : "+hostId+" MCUDID: "+mcudid);
			return;
		}

		//Devices
		String QUERY_DEVICECNTL	= "Delete from device_cntl where hostid = ?";
		String cellName = null;
		PreparedStatement deviceCntlPS		= null;

		try
		{
			String hostName = InetAddress.getLocalHost().getHostName();
			cellName = "pncell_" + hostName;
		}
		catch(Exception ex)
		{
			ex.printStackTrace();
		}
		try
		{
			conn.setAutoCommit(false);

			deviceCntlPS		= conn.prepareStatement(QUERY_DEVICECNTL);

				try
				{
					deviceCntlPS.setInt(1, hostId);
					deviceCntlPS.executeUpdate();
					conn.commit();					
				}
				catch(SQLException sqlEx)
				{
					conn.rollback();
					System.err.println("ERROR. Failed to delete hostID:" + hostId);
					sqlEx.printStackTrace();
					return;
				}

				runCellDeviceDeleteCmd(cellName, mcudid, serverPath);
		}
		catch(Exception ex)
		{
			ex.printStackTrace();
		}
		finally
		{
			try
			{
				deviceCntlPS.close();
			}
			catch(Exception ex) {}
		}
	}
	
	
	
	private static void runCellDeviceDeleteCmd(String cellName, String mcudid, String bppmPath)
	{
		String cmdPath = bppmPath + File.separator + "server" + File.separator + "bin" + File.separator;
		String queryString = "\"mc_udid : equals " + "'" + mcudid + "'\"";
		ArrayList<String> cmdList = new ArrayList<String>();
		cmdList.add("mquery.exe");
		cmdList.add("-n");
		cmdList.add(cellName);
		cmdList.add("-s");
		cmdList.add("DELETE");
		cmdList.add("-d");
		cmdList.add("-a");
		cmdList.add("BMC_ComputerSystem");
		cmdList.add("-w");
		cmdList.add(queryString);

		ProcessBuilder procBuilder = new ProcessBuilder(cmdList);
		procBuilder.directory( new File(cmdPath));
		procBuilder.redirectErrorStream(true);

		BufferedInputStream bufIS = null;

		try
		{
			Process process = procBuilder.start();
			bufIS = new BufferedInputStream( process.getInputStream());
			String buffer = "";

			int ch = -1;
			while( (ch = bufIS.read()) != -1)
			{
				buffer += (char) ch;
			}
			log("Cell command output Buffer "+buffer);
		}
		catch(IOException ioEx)
		{
			ioEx.printStackTrace();
		}
		finally
		{
			try
			{
				if( bufIS != null)
					bufIS.close();
			}catch(Exception ex) { }
		}
	}
	
	private static void deleteInstances(Connection conn, ArrayList<Integer> instList)
	{
		if(conn == null){ 
			log("Not able to delete the Instances. Connection is null. Connection: "+conn);
			return;
		}

		//Instances
		final String QUERY_ITEMCFG 				= "DELETE FROM ITEM_CFG WHERE ITEMID = ?";
		final String QUERY_FUNCPOLL 			= "DELETE FROM FUNCTIONPOLL_CNTL WHERE ITEMID = ?";
		final String QUERY_ITEMCFG_DTLS 		= "DELETE FROM ITEMCFG_DTLS WHERE ITEMID = ?";
		final String QUERY_ITEMCFG_DTLSLOG 		= "DELETE FROM ITEMCFG_DTLSLOG WHERE ITEMID = ?";
		final String QUERY_PARAMCNTL			= "DELETE FROM PARAM_CNTL WHERE ITEMID = ?";

		final String QUERY_AUTODIS_INST_CFG		= "DELETE FROM AUTODISCOVERY_INST_CFG where INSTID = ?";


		PreparedStatement itemCfgPS 		= null;
		PreparedStatement funcPollPS 		= null;
		PreparedStatement itemCfgDtlsPS 	= null;
		PreparedStatement itemCfgDtlsLogPS 	= null;
		PreparedStatement paramCntlPS 		= null;

		PreparedStatement autoDiscvPS		= null;

		int instDelCounter = 0;

		try
		{
			itemCfgPS 			= conn.prepareStatement(QUERY_ITEMCFG);
			funcPollPS 			= conn.prepareStatement(QUERY_FUNCPOLL);
			itemCfgDtlsPS 		= conn.prepareStatement(QUERY_ITEMCFG_DTLS);
			itemCfgDtlsLogPS 	= conn.prepareStatement(QUERY_ITEMCFG_DTLSLOG);
			paramCntlPS 		= conn.prepareStatement(QUERY_PARAMCNTL);

			autoDiscvPS			= conn.prepareStatement(QUERY_AUTODIS_INST_CFG);


			conn.setAutoCommit(false);

			System.out.print("Deleting Instances ... ");
			for(int i = 0; i < instList.size(); i++)
			{
				int itemID = instList.get(i);

				try
				{
					itemCfgPS.setInt(1, itemID);
					funcPollPS.setInt(1, itemID);
					itemCfgDtlsPS.setInt(1, itemID);
					itemCfgDtlsLogPS.setInt(1, itemID);
					paramCntlPS.setInt(1, itemID);
					autoDiscvPS.setInt(1, itemID);

					itemCfgPS.executeUpdate();
					funcPollPS.executeUpdate();
					itemCfgDtlsPS.executeUpdate();
					itemCfgDtlsLogPS.executeUpdate();
					paramCntlPS.executeUpdate();					
					autoDiscvPS.executeUpdate();

					conn.commit();
					instDelCounter++;
				}
				catch(SQLException sqlEx)
				{
					conn.rollback();

					System.err.println("ERROR. Failed to delete itemID" + itemID);
					sqlEx.printStackTrace();

					continue;
				}
			}
			System.out.println("Completed. Delete count: (" + instDelCounter + "/" + (instList.size()) + ")");
		}
		catch(Exception ex)
		{
			ex.printStackTrace();
		}
		finally
		{
			try
			{
				itemCfgPS.close();
			}
			catch(Exception ex) {}

			try
			{
				funcPollPS.close();
			}
			catch(Exception ex) {}

			try
			{
				itemCfgDtlsPS.close();
			}
			catch(Exception ex) {}

			try
			{
				itemCfgDtlsLogPS.close();
			}
			catch(Exception ex) {}

			try
			{
				paramCntlPS.close();
			}
			catch(Exception ex) {}

			try
			{
				autoDiscvPS.close();
			}
			catch(Exception ex) {}
		}

	}
	
	public static void usage()
	{
		System.out.println("DeleteDeviceAndAssociatedInstances <-serverpath path> <-devicename  DeviceName> ");
		System.out.println("Where options are as follows -");
		System.out.println(" serverpath - Path to the server's pw directory.");
		System.out.println(" devicename - The displayname of the device that needs to be deleted.");		
		System.exit(0);
	}
	
}
