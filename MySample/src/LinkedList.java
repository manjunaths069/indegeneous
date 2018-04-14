


//Basic node stored in a linked list
//Note that this class is not accessible outside
//of package weiss.nonstandard

class ListNode 
{

	 public Object   element;
	 public ListNode next;

	
	// Constructors
	 public ListNode( Object theElement ) {
	     this( theElement, null );
	 }
	 
	 public ListNode( Object theElement, ListNode n ) {
	     element = theElement;
	     next    = n;
	 }
 
}


/**
 * Linked list implementation of the list
 *    using a header node.
 * Access to the list is via LinkedListIterator.
 * @author ************
 * @see LinkedListIterator
 */
public class LinkedList 
{
	
	
	private ListNode header;
	/** Construct the list */
	public LinkedList()
	{
		header = new ListNode(null);
	}
	
	/**
     * Test if the list is logically empty.
     * @return true if empty, false otherwise.
     */
	
	public boolean isEmpty()
	{
		return header.next == null;
	}
	
    /**
     * Make the list logically empty.
     */
	
	public void makeEmpty()
	{
		header.next= null;
	}
	
    /**
     * Return an iterator representing the header node.
     */
	public LinkedListIterator zeroth()
	{
		return new LinkedListIterator(header);
	}
	
    /**
     * Return an iterator representing the first node in the list.
     * This operation is valid for empty lists.
     */
	
	public LinkedListIterator first()
	{
		return new LinkedListIterator(header.next);
	}
	
    /**
     * Return an iterator representing the first node in the list.
     * This operation is valid for empty lists.
     */
	
	public void insert(Object x, LinkedListIterator p)
	{ 
		if(p!=null && p.current !=null)
		{
			p.current.next= new ListNode(x,p.current.next);
		}
		
	}
	
    /**
     * Return iterator corresponding to the first node containing an item.
     * @param x the item to search for.
     * @return an iterator; iterator is not valid if item is not found.
     */
	
	public LinkedListIterator find(Object x)
	{
		ListNode itr = header.next;
		
		while( (itr.next != null) && (itr.next.element.equals(x)))
			itr = itr.next;
		
		return new LinkedListIterator(itr);
	}
	
	
    /**
     * Return iterator prior to the first node containing an item.
     * @param x the item to search for.
     * @return appropriate iterator if the item is found. Otherwise, the
     * iterator corresponding to the last element in the list is returned.
     */
	
	public LinkedListIterator findPrevious(Object x)
	{
		ListNode itr = header;
		while(itr.next != null && !(itr.next.element.equals(x)))
			itr = itr.next;
		
		return new LinkedListIterator(itr);
	}
	
	 /**
     * Remove the first occurrence of an item.
     * @param x the item to remove.
     */
	
	public void remove(Object x)
	{
		LinkedListIterator p = findPrevious(x);
		
		if(p.current.next !=null)
			p.current.next=p.current.next.next;
	}
	
	/** Simple print method */
	
	public static void printList(LinkedList list)
	{
		if(list.isEmpty())
			System.out.println("List is empty");
		else
		{
			LinkedListIterator itr = list.first();
			while(itr.isValid())
			{
				System.out.println(itr.retrieve()+" ");
				itr.advance();
			}
		}
		System.out.println();
	}
	
    // In this routine, LinkedList and LinkedListIterator are the
    // classes written in Section 17.2.
	
    public static int listSize( LinkedList theList ) {
        LinkedListIterator itr;
        int size = 0;
        
        for( itr = theList.first(); itr.isValid(); itr.advance() )
            size++;
        
        return size;
    }
    
    public static void main( String [ ] args ) {
        LinkedList         theList = new LinkedList( );
        LinkedListIterator theItr;
        int i;
        
        theItr = theList.zeroth( );
        printList( theList );
        
        for( i = 0; i < 10; i++ ) {
            theList.insert( new Integer( i ), theItr );
            printList( theList );
            theItr.advance( );
        }
        System.out.println( "Size was: " + listSize( theList ) );
        
        for( i = 0; i < 10; i += 2 )
            theList.remove( new Integer( i ) );
        
        for( i = 0; i < 10; i++ )
            if( ( i % 2 == 0 ) == ( theList.find( new Integer( i ) ).isValid( ) ) )
                System.out.println( "Find fails!" );
        
        System.out.println( "Finished deletions" );
        printList( theList );
    }
	

}


