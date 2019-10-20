package org.eclipse.p4lang;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URI;
import java.text.MessageFormat;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.preference.PreferenceDialog;
import org.eclipse.lsp4e.server.StreamConnectionProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.PreferencesUtil;

import com.google.gson.Gson;
import com.google.gson.stream.JsonReader;

public class P4LanguageServer implements StreamConnectionProvider {

	private static boolean hasCancelledSetup = false;
	private Process process;

	@Override
	public void start() throws IOException {
		String p4ls = getP4LS();
		if ((p4ls.isEmpty() || !setSystemProperties())) {
			showSetupP4Notification();
			return;
		}
		this.process = P4langPlugin.getProcessForCommand(p4ls.split("[ ]+"));
	}

	@Override
	public String toString() {
		return "P4 Language Server: " + super.toString();
	}

	@Override
	public Object getInitializationOptions(URI rootUri) {
		final String settingsPath = "nofile";
		final File settingsFile = new File(settingsPath);
		final Gson gson = new Gson();
		try (JsonReader reader = new JsonReader(new FileReader(settingsFile))) {
			return gson.fromJson(reader, HashMap.class);
		} catch (FileNotFoundException e) {
			P4langPlugin.getDefault().getLog().log(new Status(IStatus.INFO,
					P4langPlugin.getDefault().getBundle().getSymbolicName(),
					MessageFormat.format("Configuration {0} not found", settingsPath)));
		} catch (Throwable e) {
			P4langPlugin.getDefault().getLog().log(new Status(IStatus.ERROR,
					P4langPlugin.getDefault().getBundle().getSymbolicName(),
					MessageFormat.format("Configuration error {0}, {1}", settingsPath, e)));
		}
		return getDefaultInitializationOptions();
	}

	@Override
	public InputStream getInputStream() {
		P4langPlugin.getDefault().getLog().log(new Status(IStatus.INFO,
				P4langPlugin.getDefault().getBundle().getSymbolicName(),
				MessageFormat.format("P4LSD process isAlive {0}", process.isAlive())));
		return process == null ? null : process.getInputStream();
	}

	@Override
	public OutputStream getOutputStream() {
		return process == null ? null : process.getOutputStream();
	}

	@Override
	public void stop() {
		if (process != null)
			process.destroy();
	}

	@Override
	public InputStream getErrorStream() {
		return process == null ? null : process.getErrorStream();
	}

	private static synchronized void setHasCancelledSetup(Boolean newValue) {
		hasCancelledSetup = newValue;
	}

	private static Map<String, Object> getDefaultInitializationOptions() {
		final Map<String, Object> initializationSettings = new HashMap<>();
		initializationSettings.put("clippy_preference", "on"); //$NON-NLS-1$//$NON-NLS-2$
		return Collections.singletonMap("settings", Collections.singletonMap("p4lang", initializationSettings)); //$NON-NLS-1$ //$NON-NLS-2$
	}

	private boolean setSystemProperties() {
		return true;
		/*
		 * String sysrootPath = getSysRoot(); if (!sysrootPath.isEmpty()) {
		 * System.setProperty("SYS_ROOT", sysrootPath);
		 * System.setProperty("LD_LIBRARY_PATH", sysrootPath + "/lib"); String sysRoot =
		 * System.getProperty("SYS_ROOT"); String ldLibraryPath =
		 * System.getProperty("LD_LIBRARY_PATH"); if (!(sysRoot == null ||
		 * sysRoot.isEmpty() || ldLibraryPath == null || ldLibraryPath.isEmpty())) {
		 * return true; } } P4langPlugin.
		 * logError("Was unable to set the `SYS_ROOT` and `LD_LIBRARY_PATH` environment variables. "
		 * + "Please do so manually."); return false;
		 */
	}

	private String getP4LS() {
		String command = new String();
		boolean wereSystemPropertiesSet = setSystemProperties();
		P4langPlugin plugin = P4langPlugin.getDefault();
		IPreferenceStore preferenceStore = plugin.getPreferenceStore();
		String p4lsd = preferenceStore.getString(P4langPreferenceInitializer.p4lsdPathPreference);
		if ((p4lsd.isEmpty() || !wereSystemPropertiesSet)) {
			P4langPlugin.logError("P4 Language Server not found. Update in P4 preferences.");
			showSetupP4Notification();
			return command;
		}
		command += p4lsd;
		String args = preferenceStore.getString(P4langPreferenceInitializer.p4lsdArgsPreference);
		if (!args.isEmpty()) {
			command += " " + args;
		}
		return command;
	}

	private void showSetupP4Notification() {
		if (hasCancelledSetup) {
			return;
		}
		Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
		int dialogResponse = MessageDialog.open(MessageDialog.CONFIRM, shell, "P4 Support Not Found",
				"Requirments for P4 editing were not found. "
						+ "Install the required components or input their paths in the P4 Preferences.",
				SWT.NONE, "Open Preferences", "Cancel");
		if (dialogResponse == 0) {
			PreferenceDialog preferenceDialog = PreferencesUtil.createPreferenceDialogOn(shell,
					P4langPreferencePage.PAGE_ID, new String[] { P4langPreferencePage.PAGE_ID }, null);
			preferenceDialog.setBlockOnOpen(true);
			preferenceDialog.open();
		} else {
			setHasCancelledSetup(true);
		}
	}
}
