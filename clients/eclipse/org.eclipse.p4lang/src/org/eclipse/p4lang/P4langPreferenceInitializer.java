package org.eclipse.p4lang;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

public class P4langPreferenceInitializer extends AbstractPreferenceInitializer {

	private static final IPreferenceStore STORE = P4langPlugin.getDefault().getPreferenceStore();
	public static final String p4lsdPathPreference = "p4lang.p4lsdPath";
	public static final String p4lsdArgsPreference = "p4lang.p4lsdArgs";
	public static final String WORKING_DIRECTORY_PREFERENCE = "p4lang.workingDirectory"; //$NON-NLS-1$

	@Override
	public void initializeDefaultPreferences() {
		STORE.setDefault(p4lsdPathPreference, getP4LSPathBestGuess());
	}

	private String findCommandPath(String command) {
		try {
			ProcessBuilder builder = new ProcessBuilder("which", command);
			Process process = builder.start();
			if (process.waitFor() == 0) {
				try (BufferedReader in = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
					return in.readLine();
				}
			}
		} catch (IOException | InterruptedException e) {
			// Errors caught with empty return
		}
		return "";
	}

	private String getP4LSPathBestGuess() {
		String command = findCommandPath("p4lsd");
		if (command.isEmpty()) {
			File possibleCommandFile = new File(System.getProperty("user.home") + "/local/bin/p4lsd");
			if (possibleCommandFile.exists() && possibleCommandFile.isFile() && possibleCommandFile.canExecute()) {
				return possibleCommandFile.getAbsolutePath();
			}
		}
		return command;
	}

	/*
	private String getSysrootPathBestGuess() {
		File p4c = new File(findCommandPath("p4c"));
		if (!(p4c.exists() && p4c.isFile() && p4c.canExecute())) {
			p4c = new File(System.getProperty("user.home") + "/local/bin/p4c");
		}
		if (!(p4c.exists() && p4c.isFile() && p4c.canExecute())) {
			return "";
		}
		String[] command = new String[] { p4c.getAbsolutePath(), "--print", "sysroot" };
		try {
			Process process = Runtime.getRuntime().exec(command);
			try (BufferedReader in = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
				return in.readLine();
			}
		} catch (IOException e) {
			return "";
		}
	}
	*/
}
