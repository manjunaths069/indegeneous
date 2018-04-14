

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class DBConnection {

	private static DBConnection dbConFactory;

	final static String AUTH_KEY = "SET TEMPORARY OPTION CONNECTION_AUTHENTICATION='Company=PROACTIVENET;Application=PROACTIVE;Signature=000fa55157edb8e14d818eb4fe3db41447146f1571g0aa9c00d3e52e7e5f8de89c5566f6c594093ac1a'";

	final static String login_procedure = "set temporary option Allow_nulls_by_default = 'ON'";

	static {
		try {
			Class.forName("com.sybase.jdbc3.jdbc.SybDriver");
		} catch (ClassNotFoundException e) {

			e.printStackTrace();
			System.exit(1);
		}

	}

	private DBConnection(){
		
	}

	public static DBConnection getInstance() {

		if (dbConFactory == null) {
			dbConFactory = new DBConnection();
		}

		return dbConFactory;

	}

	public Connection getConnection()throws Exception {
		try {
			String hostname = InetAddress.getLocalHost().getHostName();
			Connection con = DriverManager.getConnection("jdbc:sybase:Tds:"
					+ hostname + ":2638", "dba", "pronet");

			con.createStatement().executeUpdate(AUTH_KEY);
			con.createStatement().executeUpdate(login_procedure);
			con.commit();
			return con;
		} catch (SQLException e) {

			throw e;
		} catch (Exception ee) {
			throw ee;
		}

		
	}

}
