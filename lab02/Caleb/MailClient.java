/****************************************************************************
* Program:
*    Lab MailClient, Computer Communication and Networking
*    Brother Jones, CS 460
* Author:
*    Caleb Davidson
* Summary:
*    Runs a simple gui to send an email . Asks the user for a from address
*    a to address, subject, and message. Handles SMTP connection logic
*    and print commands and replies to the console.
************************************************************************** */

import java.net.*;
import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.text.*;

/**
 * A simple mail client with a GUI for sending mail.
 */
public class MailClient extends Frame
{
   /* The stuff for the GUI. */
   private Button btSend = new Button("Send");
   private Button btClear = new Button("Clear");
   private Button btQuit = new Button("Quit");
   private Label fromLabel = new Label("From:");
   private TextField fromField = new TextField("", 40);
   private Label toLabel = new Label("To:");
   private TextField toField = new TextField("", 40);
   private Label subjectLabel = new Label("Subject:");
   private TextField subjectField = new TextField("", 40);
   private Label messageLabel = new Label("Message:");
   private TextArea messageText = new TextArea(10, 40);
   
   /**
    * Create a new MailClient window with fields for entering all
    * the relevant information (From, To, Subject, and message).
    */
   public MailClient()
   {
      super ("Java Mailclient");
      /* Create panels for holding the fields. To make it look nice,
      *  create an extra panel for holding all the child panels. */
      Panel fromPanel = new Panel(new BorderLayout());
      Panel toPanel = new Panel(new BorderLayout());
      Panel subjectPanel = new Panel(new BorderLayout());
      Panel messagePanel = new Panel(new BorderLayout());
      fromPanel.add(fromLabel, BorderLayout.WEST);
      fromPanel.add(fromField, BorderLayout.CENTER);
      toPanel.add(toLabel, BorderLayout.WEST);
      toPanel.add(toField, BorderLayout.CENTER);
      subjectPanel.add(subjectLabel, BorderLayout.WEST);
      subjectPanel.add(subjectField, BorderLayout.CENTER);
      messagePanel.add(messageLabel, BorderLayout.NORTH);
      messagePanel.add(messageText, BorderLayout.CENTER);
      Panel fieldPanel = new Panel(new GridLayout(0, 1));
      fieldPanel.add(fromPanel);
      fieldPanel.add(toPanel);
      fieldPanel.add(subjectPanel);
      /* Create a panel for the buttons and add listeners to the buttons. */
      Panel buttonPanel = new Panel(new GridLayout(1, 0));
      btSend.addActionListener(new SendListener());
      btClear.addActionListener(new ClearListener());
      btQuit.addActionListener(new QuitListener());
      buttonPanel.add(btSend);
      buttonPanel.add(btClear);
      buttonPanel.add(btQuit);
      /* Add, pack, and show. */
      add(fieldPanel, BorderLayout.NORTH);
      add(messagePanel, BorderLayout.CENTER);
      add(buttonPanel, BorderLayout.SOUTH);
      pack();
      setVisible(true);
   }
   
   static public void main(String argv[])
   {
      new MailClient();
   }
   
   /* Handler for the Send-button. */
   class SendListener implements ActionListener
   {
      public void actionPerformed(ActionEvent event)
      {
         System.out.println("Preparing to send mail");
         /* First, check that we have the sender and recipient. */
         if ((fromField.getText()).equals(""))
         {
            System.out.println("Need sender!");
            return;
         }
         if ((toField.getText()).equals(""))
         {
            System.out.println("Need recipient!");
            return;
         }
         /* Create the message */
         Message mailMessage = new Message(fromField.getText(),
                                           toField.getText(),
                                           subjectField.getText(),
                                           messageText.getText());
         
         /* Check that the message is valid, i.e., sender and
         *  recipient addresses look ok. */
         if (!mailMessage.isValid())
         {
            return;
         }
         
         /* Create the envelope, open the connection and try to
         *  Send the message. */
         Envelope envelope = new Envelope(mailMessage);
         try
         {
            System.out.println("Opening Connection");
            SMTPConnection connection = new SMTPConnection(envelope);
            System.out.println("Sending Envelope");
            connection.send(envelope);
            System.out.println("Closing Connection");
            connection.close();
         }
         catch (IOException error)
         {
            System.out.println("Sending failed: " + error);
            return;
         }
         System.out.println("Mail sent successfully!");
      }
   }
   
   /* Clear the fields on the GUI. */
   class ClearListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         System.out.println("Clearing fields");
         fromField.setText("");
         toField.setText("");
         subjectField.setText("");
         messageText.setText("");
      }
   }

   /* Quit. */
   class QuitListener implements ActionListener
   {
      public void actionPerformed(ActionEvent e)
      {
         System.exit(0);
      }
   }
}

/**
 * Mail message.
 */
class Message
{
   /* The headers and the body of the message. */
   public String Headers;
   public String Body;

   /* Sender and recipient. With these, we don't need to extract
   *  them from the headers. */
   private String From;
   private String To;

   /* To make it look nicer */
   private static final String CRLF = "\r\n";

   /**
    * Create the message object by inserting the required headers
    * from RFC 822 (From, To, Date).
    */
   public Message(String from, String to, String subject, String text)
   {
      /* Remove whitespace */
      From = from.trim();
      To = to.trim();
      Headers = "From: " + From + CRLF;
      Headers += "To: " + To + CRLF;
      Headers += "Subject: " + subject.trim() + CRLF;
      /* A close approximation of the required format. Unfortunately
      *  only GMT. */
      SimpleDateFormat format =
               new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss 'GMT'");
      String dateString = format.format(new Date());
      Headers += "Date: " + dateString + CRLF;
      Body = text;
   }

   /**
    * Getter for the From string of the message header
    */
   public String getFrom()
   {
      return From;
   }

   /**
    * Getter for the To string of the message header
    */
   public String getTo()
   {
      return To;
   }

   /**
    * Check whether the message is valid. In other words, check that
    * both sender and recipient contain only one @-sign.
    */
   public boolean isValid()
   {
      int fromat = From.indexOf('@');
      int toat = To.indexOf('@');
      if (fromat < 1 || (From.length() - fromat) <= 1)
      {
         System.out.println("Sender address is invalid");
         return false;
      }
      if (toat < 1 || (To.length() - toat) <= 1)
      {
         System.out.println("Recipient address is invalid");
         return false;
      }
      if (fromat != From.lastIndexOf('@'))
      {
         System.out.println("Sender address is invalid");
         return false;
      }
      if (toat != To.lastIndexOf('@'))
      {
         System.out.println("Recipient address is invalid");
         return false;
      }
      return true;
   }

   /**
    *For printing the message.
    */
   public String toString()
   {
      String res;
      res = Headers + CRLF;
      res += Body;
      return res;
   }
}


/**
 * SMTP envelope for one mail message.
 */
class Envelope
{
   /* SMTP-sender of the message (in this case, contents of
   From-header. */
   public String Sender;
   /* SMTP-recipient, or contents of To-header. */
   public String Recipient;
   /* Target MX-host */
   public String DestHost;
   public InetAddress DestAddr;
   /* The actual message */
   public Message Message;

   /**
    * Creates the envelope.
    */
   public Envelope(Message message)
   {
      /* Get sender and recipient. */
      Sender = message.getFrom();
      Recipient = message.getTo();
      /* Get message. We must escape the message to make sure that
      there are no single periods on a line. This would mess up
      sending the mail. */
      Message = escapeMessage(message);
      /* Get the hostname part of the recipient. It should be the
      name of the MX-host for the recipient's domain. */
      int atsign = Recipient.lastIndexOf('@');
      DestHost = Recipient.substring(atsign + 1);
      /* Map the name into an IP-address */
      try
      {
         DestAddr = InetAddress.getByName(DestHost);
      }
      catch (UnknownHostException e)
      {
         System.out.println("Unknown host: " + DestHost);
         System.out.println(e);
         return;
      }
      return;
   }

   /**
    * Escapes the message by doubling all periods at the beginning of
    * a line.
    */
   private Message escapeMessage(Message message)
   {
      String escapedBody = "";
      String token;
      StringTokenizer parser = new StringTokenizer(message.Body, "\n", true);
      while (parser.hasMoreTokens())
      {
         token = parser.nextToken();
         if (token.startsWith("."))
         {
            token = "." + token;
         }
         escapedBody += token;
      }
      message.Body = escapedBody;
      return message;
   }

   /**
    * For printing the envelope. Only for debug.
    */
   public String toString()
   {
      String res = "Sender: " + Sender + '\n';
      res += "Recipient: " + Recipient + '\n';
      res += "MX-host: " + DestHost + ", address: " + DestAddr + '\n';
      res += "Message:" + '\n';
      res += Message.toString();
      return res;
   }
}

/**
 * Open an SMTP connection to a remote machine and send one mail.
 */
class SMTPConnection
{
   /* The socket to the server */
   private Socket connection;
   /* Streams for reading and writing the socket */
   private BufferedReader fromServer;
   private DataOutputStream toServer;
   // Useful constants
   private static final int SMTP_PORT = 25;
   private static final String CRLF = "\r\n";
   /* Are we connected? Used in close() to determine what to do. */
   private boolean isConnected = false;

   /**
    * Create an SMTPConnection object. Create the socket and the
    * associated streams. Initialize SMTP connection.
    */
   public SMTPConnection(Envelope envelope) throws IOException
   {
      connection = new Socket(envelope.DestAddr.getHostName(), SMTP_PORT);
      fromServer = new BufferedReader(new InputStreamReader(connection.getInputStream()));
      toServer = new DataOutputStream(connection.getOutputStream());

      /* Read a line from server and check that the reply code is 220.
      If not, throw an IOException. */
      String reply = fromServer.readLine();
      if (parseReply(reply) != 220) {
         System.out.println("Could not connect: " + reply);
         throw new IOException();
      }

      /* SMTP handshake. We need the name of the local machine.
      Send the appropriate SMTP handshake command. */
      String localhost = InetAddress.getLocalHost().getHostName();
      sendCommand("HELO " + localhost, 250);

      isConnected = true;
   }
   
   /**
    * Send the message. Write the correct SMTP-commands in the
    * correct order. No checking for errors, just throw them to the
    * caller.
    */
   public void send(Envelope envelope) throws IOException
   {
      /* Send all the necessary commands to send a message. Call
      sendCommand() to do the dirty work. Do _not_ catch the
      exception thrown from sendCommand(). */
      sendCommand("MAIL FROM:<" + envelope.Sender + "> ", 250);
      sendCommand("RCPT TO:<" + envelope.Recipient + "> ", 250);
      sendCommand("DATA", 354);
      sendCommand(envelope.Message.toString() + CRLF + ".", 250);
    }
   
   /**
    * Close the connection. First, terminate on SMTP level, then
    * close the socket.
    */
   public void close()
   {
      isConnected = false;
      try
      {
         sendCommand("QUIT", 221);
         connection.close();
      }
      catch (IOException e)
      {
         System.out.println("Unable to close connection: " + e);
         isConnected = true;
      }
   }

   /**
    * Send an SMTP command to the server. Check that the reply code
    * is what is is supposed to be according to RFC 821.
    */
   private void sendCommand(String command, int rc) throws IOException
   {
      System.out.println("Command Sent: " + command);
      /* Write command to server and read reply from server. */
      toServer.writeBytes(command + CRLF);
      /* Check that the server's reply code is the same as the
      parameter rc. If not, throw an IOException. */
      if (parseReply(fromServer.readLine()) != rc) {
         throw new IOException();
      }
   }

   /**
    * Parse the reply line from the server. Returns the reply code.
    */
   private int parseReply(String reply)
   {
      System.out.println("Server Reply: " + reply);
      // Returns the status code for the line
      // It should be the first element on the line
      return new Integer(reply.split(" ")[0]);
   }
   
   /**
    * Destructor. Closes the connection if something bad happens.
    */
   protected void finalize() throws Throwable
   {
      if (isConnected)
      {
         close();
      }
      super.finalize();
   }
}

