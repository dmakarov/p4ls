package org.eclipse.p4lang.tests;

import java.io.IOException;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.internal.genericeditor.ExtensionBasedTextEditor;
import org.junit.Assert;
import org.junit.Test;

@SuppressWarnings("restriction")
public class TestIDEIntegration extends AbstractP4langTest {

	@Test
	public void testP4EditorAssociation() throws IOException, CoreException {
		IWorkbenchPage activePage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		IEditorPart editor = IDE.openEditor(activePage, getProject("basic").getFolder("src").getFile("main.p4"));
		Assert.assertTrue(editor instanceof ExtensionBasedTextEditor);
	}

}
