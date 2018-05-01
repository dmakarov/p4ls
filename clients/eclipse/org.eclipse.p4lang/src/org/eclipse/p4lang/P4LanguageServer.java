package org.eclipse.p4lang;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.preference.PreferenceDialog;
import org.eclipse.lsp4e.server.ProcessStreamConnectionProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.PreferencesUtil;

public class P4LanguageServer extends ProcessStreamConnectionProvider {

	private static boolean hasCancelledSetup = false;

	public P4LanguageServer() {
		List<String> commands = getP4LS();
		if (!commands.isEmpty()) {
			setCommands(commands);
			setWorkingDirectory(Platform.getLocation().toOSString());
		}
	}

	@Override
	public String toString() {
		return "P4 Language Server: " + super.toString();
	}

	private boolean setSystemProperties() {
		return true;
		/*
		String sysrootPath = getSysRoot();
		if (!sysrootPath.isEmpty()) {
			System.setProperty("SYS_ROOT", sysrootPath);
			System.setProperty("LD_LIBRARY_PATH", sysrootPath + "/lib");
			String sysRoot = System.getProperty("SYS_ROOT");
			String ldLibraryPath = System.getProperty("LD_LIBRARY_PATH");
			if (!(sysRoot == null || sysRoot.isEmpty() || ldLibraryPath == null || ldLibraryPath.isEmpty())) {
				return true;
			}
		}
		P4langPlugin.logError("Was unable to set the `SYS_ROOT` and `LD_LIBRARY_PATH` environment variables. Please do so manually.");
		return false;
        */
	}

	private List<String> getP4LS() {
		List<String> command = new ArrayList<>();
		boolean wereSystemPropertiesSet = setSystemProperties();
		P4langPlugin plugin = P4langPlugin.getDefault();
		IPreferenceStore preferenceStore = plugin.getPreferenceStore();
		String p4lsd = preferenceStore.getString(P4langPreferenceInitializer.p4lsdPathPreference);
		if ((p4lsd.isEmpty() || !wereSystemPropertiesSet)) {
			P4langPlugin.logError("P4 Language Server not found. Update in P4 preferences.");
			showSetupP4Notification();
			return command;
		}
		command.add(p4lsd);
		String args = preferenceStore.getString(P4langPreferenceInitializer.p4lsdArgsPreference);
		if (!args.isEmpty()) {
			for (final String arg : args.split("[ ]+")) {
				command.add(arg);
			}
		}
		return command;
	}

	private void showSetupP4Notification() {
		if (hasCancelledSetup) {
			return;
		}
		Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
		int dialogResponse = MessageDialog.open(MessageDialog.CONFIRM, shell, "P4 Support Not Found", "Requirments for P4 editing were not found. Install the required components or input their paths in the P4 Preferences.", SWT.NONE, "Open Preferences", "Cancel");
		if (dialogResponse == 0) {
			PreferenceDialog preferenceDialog = PreferencesUtil.createPreferenceDialogOn(shell, P4langPreferencePage.PAGE_ID, new String[] { P4langPreferencePage.PAGE_ID }, null);
			preferenceDialog.setBlockOnOpen(true);
			preferenceDialog.open();
		} else {
			hasCancelledSetup = true;
		}
	}
}
