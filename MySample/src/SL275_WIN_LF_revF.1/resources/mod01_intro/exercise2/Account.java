/*
 * Account.java
 *
 * Created on November 9, 2005, 12:46 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

/**
 *
 * @author student````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````
 */
public class Account {
    private double balance;
    
    /** Creates a new instance of Account */
    public Account(double initBalance) {
        balance = initBalance;
    }
    
    public double getBalance() {
        return balance;
    }
    
    public void deposit(double amt) {
        balance = balance + amt;
    }
    
    public void withdraw(double amt) {
        if (balance >= amt) {
            balance = balance - amt;
        }
    }
}
