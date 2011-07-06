/*******************************************************************************
 * Copyright (c) 2010 - 2011 by Timotei Dolean <timotei21@gmail.com>
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/
package org.wesnoth;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.text.SimpleDateFormat;
import java.util.Date;

import javax.swing.JOptionPane;

import org.eclipse.core.runtime.IStatus;
import org.wesnoth.utils.GUIUtils;
import org.wesnoth.utils.WorkspaceUtils;


/**
 * A class that logs activities in a file
 */
public class Logger {

    private static class LoggerInstance{
        private static Logger instance_ = new Logger();
    }

	private Logger() { }

	private BufferedWriter logWriter_;

	public static Logger getInstance()
	{
		return LoggerInstance.instance_;
	}

	/**
	 * Starts the logger - creates the log file in the temporary directory
	 */
	public void startLogger()
	{
		if (logWriter_ != null)
			return;
		try
		{
			String logFilePath = String.format("%s/logs/log%s.txt",  //$NON-NLS-1$
				WorkspaceUtils.getTemporaryFolder(), WorkspaceUtils.getRandomFileName());

			if (WorkspaceUtils.getTemporaryFolder() == null)
			    throw new IOException("Could not create the temporary folder."); //$NON-NLS-1$

			new File(WorkspaceUtils.getTemporaryFolder() + "/logs/").mkdirs();  //$NON-NLS-1$

			logWriter_ = new BufferedWriter(new FileWriter(logFilePath));
			log("Logging started."); //$NON-NLS-1$
			log("Error codes: 1 - INFO, 2 - WARNING, 4 - ERROR"); //$NON-NLS-1$
		} catch (IOException e)
		{
		    JOptionPane.showMessageDialog(null, "There was an error trying to open the log." + e.getMessage()); //$NON-NLS-1$
			e.printStackTrace();
		}
	}

	/**
	 * Stops the logger
	 */
	public void stopLogger()
	{
		if (logWriter_ == null)
			return;
		try
		{
			log("Logging ended."); //$NON-NLS-1$
			logWriter_.close();
		} catch (IOException e)
		{
			e.printStackTrace();
		}
	}

	/**
	 * Prints a message to the error log (severity: info)
	 * @param message the message to print
	 */
	public void log(String message)
	{
		log(message, IStatus.INFO);
	}

	/**
	 * Logs a warning message
	 * @param message
	 */
	public void logWarn(String message)
	{
		log(message,IStatus.WARNING);
	}
	/**
	 * Logs an error message
	 * @param message
	 */
	public void logError(String message)
	{
		log(message,IStatus.ERROR);
	}

	/**
	 * Logs the specified exception, providing the stacktrace to the console
	 * @param e
	 */
	public void logException(Exception e)
	{
		if (e == null)
			return;
		e.printStackTrace();

		// put the stack trace in a string
		StringWriter sw = new StringWriter();
		PrintWriter pw = new PrintWriter(sw);
		e.printStackTrace(pw);

		log(e.getLocalizedMessage(), IStatus.ERROR);
		log(sw.toString(), IStatus.ERROR);
	}

	/**
	 * Prints a message to the error log with the specified severity
	 * @param message the message to print
	 * @param severity the severity level from IStatus enum
	 */
	public void log(String message, int severity)
	{
		if (logWriter_ != null)
		{
			try
			{
				logWriter_.write(String.format("%s | %d | %s\n",  //$NON-NLS-1$
						new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date()),  //$NON-NLS-1$
						severity,  message));
				logWriter_.flush();
			} catch (IOException e)
			{
				e.printStackTrace();
			}
		}
		System.out.println(message);
	}

	/**
	 * Logs the message (severity: info) showing also a messagebox to the user
	 * @param message
	 * @param guiMessage
	 */
	public void log(String message, String guiMessage)
	{
		log(message,IStatus.INFO);
		GUIUtils.showInfoMessageBox(guiMessage);
	}
}
