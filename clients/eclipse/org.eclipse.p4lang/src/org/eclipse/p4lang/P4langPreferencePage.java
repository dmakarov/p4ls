package org.eclipse.p4lang;

import static org.eclipse.swt.events.SelectionListener.widgetSelectedAdapter;

import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;

public class P4langPreferencePage extends PreferencePage implements IWorkbenchPreferencePage {

	public static final String PAGE_ID = "org.eclipse.P4lang.preferencePage";

	private IPreferenceStore store;

	@Override
	public void init(IWorkbench workbench) {
		store = doGetPreferenceStore();
	}

	@Override
	protected Control createContents(Composite parent) {
		Composite container = new Composite(parent, SWT.NULL);
		container.setLayout(new GridLayout(2, false));
		createContent(container);
		initializeContent();
		return container;
	}

	@Override
	protected IPreferenceStore doGetPreferenceStore() {
		return P4langPlugin.getDefault().getPreferenceStore();
	}

	@Override
	protected void performDefaults() {
		p4lsdPathText.setText(store.getDefaultString(P4langPreferenceInitializer.p4lsdPathPreference));
		p4lsdArgsText.setText(store.getDefaultString(P4langPreferenceInitializer.p4lsdArgsPreference));
		super.performDefaults();
	}

	@Override
	public boolean performOk() {
		store.setValue(P4langPreferenceInitializer.p4lsdPathPreference, p4lsdPathText.getText());
		store.setValue(P4langPreferenceInitializer.p4lsdArgsPreference, p4lsdArgsText.getText());
		return true;
	}

	private Label p4lsdLabel;
	private Text p4lsdPathText;
	private Button p4lsdBrowseButton;

	private Label argsLabel;
	private Text p4lsdArgsText;

	private void createContent(Composite container) {
		Composite parent = new Composite(container, SWT.NULL);
		parent.setLayout(new GridLayout(3, false));
		parent.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));

		GridData labelIndent = new GridData(SWT.FILL, SWT.CENTER, true, false, 3, 1);
		labelIndent.horizontalIndent = 25;

		GridData textIndent = new GridData(SWT.FILL, SWT.CENTER, true, false);
		textIndent.horizontalIndent = 50;
		textIndent.widthHint = convertWidthInCharsToPixels(50);

		p4lsdLabel = new Label(parent, SWT.NONE);
		p4lsdLabel.setText("Path to the P4 Language Server (p4lsd):");
		p4lsdLabel.setLayoutData(labelIndent);

		p4lsdPathText = new Text(parent, SWT.BORDER);
		p4lsdPathText.setLayoutData(textIndent);
		p4lsdPathText.addModifyListener(e -> {
			setValid(true);
		});

		p4lsdBrowseButton = new Button(parent, SWT.NONE);
		p4lsdBrowseButton.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false));
		p4lsdBrowseButton.setText("Browse...");
		p4lsdBrowseButton.addSelectionListener(widgetSelectedAdapter(e -> {
			FileDialog dialog = new FileDialog(p4lsdBrowseButton.getShell());
			String path = dialog.open();
			if (path != null) {
				p4lsdPathText.setText(path);
			}
		}));

		argsLabel = new Label(parent, SWT.NONE);
		argsLabel.setText("Command line options for p4lsd:");
		argsLabel.setLayoutData(labelIndent);

		p4lsdArgsText = new Text(parent, SWT.BORDER);
		p4lsdArgsText.setLayoutData(textIndent);
		p4lsdArgsText.addModifyListener(e -> {
			setValid(true);
		});
	}

	private void initializeContent() {
		p4lsdPathText.setText(store.getString(P4langPreferenceInitializer.p4lsdPathPreference));
		p4lsdArgsText.setText(store.getString(P4langPreferenceInitializer.p4lsdArgsPreference));
	}
}
