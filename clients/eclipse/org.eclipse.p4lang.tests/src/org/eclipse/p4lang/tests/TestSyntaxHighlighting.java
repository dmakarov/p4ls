package org.eclipse.p4lang.tests;

import java.io.IOException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.tests.harness.util.DisplayHelper;
import org.junit.Assert;
import org.junit.Test;

public class TestSyntaxHighlighting extends AbstractP4langTest {

	@Test
	public void testP4langSyntaxHighlighting() throws CoreException, IOException {
		IFile p4File = getProject("basic").getFolder("src").getFile("main.p4");
		TextEditor editor = (TextEditor) IDE.openEditor(
				PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage(), p4File,
				"org.eclipse.ui.genericeditor.GenericEditor");
		StyledText editorTextWidget = (StyledText) editor.getAdapter(Control.class);
		new DisplayHelper() {
			@Override
			protected boolean condition() {
				return editorTextWidget.getStyleRanges().length > 1;
			}
		}.waitForCondition(editorTextWidget.getDisplay(), 4000);
		Assert.assertTrue("There should be multiple styles in editor", editorTextWidget.getStyleRanges().length > 1);
	}

}
