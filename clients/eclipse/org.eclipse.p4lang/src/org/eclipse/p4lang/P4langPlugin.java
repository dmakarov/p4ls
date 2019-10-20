package org.eclipse.p4lang;

import java.io.File;
import java.io.IOException;
import java.text.MessageFormat;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

public class P4langPlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static final String PLUGIN_ID = "org.eclipse.p4lang"; //$NON-NLS-1$

	// The shared instance
	private static P4langPlugin plugin;

	public P4langPlugin() {
	}

	@Override
	public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
	}

	@Override
	public void stop(BundleContext context) throws Exception {
		plugin = null;
		super.stop(context);
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static P4langPlugin getDefault() {
		return plugin;
	}

	public static void logError(Throwable t) {
		plugin.getLog().log(new Status(IStatus.ERROR, PLUGIN_ID, t.getMessage(), t));
	}
	
	public static void logError(final String m) {
		plugin.getLog().log(new Status(IStatus.ERROR, plugin.getBundle().getSymbolicName(), m));
	}
	
	public static Process getProcessForCommand(String... commandStrings) throws IOException {
		ProcessBuilder builder = new ProcessBuilder(commandStrings);
		//builder.directory(getWorkingDirectoryFromPreferences());
		getDefault().getLog().log(new Status(IStatus.INFO,
		getDefault().getBundle().getSymbolicName(),
		MessageFormat.format("Starting a new process for command {0} {1} {2}", (Object[])commandStrings)));
		return builder.start();
	}

	private static File getWorkingDirectoryFromPreferences() {
		String wdString = getDefault().getPreferenceStore().getString(P4langPreferenceInitializer.WORKING_DIRECTORY_PREFERENCE);
		if (wdString == null) {
			return null;
		}
		File wdFile = new File(wdString);
		if (wdFile.exists() && wdFile.isDirectory()) {
			return wdFile;
		}
		return null;
	}

}
