package org.eclipse.p4lang;

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
}
