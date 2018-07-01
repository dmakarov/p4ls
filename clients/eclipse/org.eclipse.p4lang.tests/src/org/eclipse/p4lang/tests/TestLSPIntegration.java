package org.eclipse.p4lang.tests;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.util.List;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.TimeUnit;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.lsp4e.LanguageServiceAccessor;
import org.eclipse.lsp4j.CompletionItem;
import org.eclipse.lsp4j.CompletionList;
import org.eclipse.lsp4j.CompletionParams;
import org.eclipse.lsp4j.Position;
import org.eclipse.lsp4j.TextDocumentIdentifier;
import org.eclipse.lsp4j.jsonrpc.messages.Either;
import org.eclipse.lsp4j.services.LanguageServer;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.tests.harness.util.DisplayHelper;
import org.junit.Assert;
import org.junit.Test;

public class TestLSPIntegration extends AbstractP4langTest {

	@Test
	public void testLSFound() throws Exception {
		IProject project = getProject("basic");
		IFile p4File = project.getFolder("src").getFile("main.p4");
		CompletableFuture<LanguageServer> languageServer = LanguageServiceAccessor
				.getInitializedLanguageServers(p4File, capabilities -> capabilities.getHoverProvider() != null)
				.iterator().next();
		String uri = p4File.getLocationURI().toString();
		Either<List<CompletionItem>, CompletionList> completionItems = languageServer.get(1, TimeUnit.MINUTES)
				.getTextDocumentService()
				.completion(new CompletionParams(new TextDocumentIdentifier(uri), new Position(1, 4)))
				.get(1, TimeUnit.MINUTES);
		Assert.assertNotNull(completionItems);
	}

	@Test
	public void testLSWorks() throws Exception {
		IProject project = getProject("basic_errors");
		IWorkbenchPage activePage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		IFile p4File = project.getFolder("src").getFile("main.p4");
		IEditorPart editor = IDE.openEditor(activePage, p4File);
		new DisplayHelper() {
			@Override
			protected boolean condition() {
				try {
					return p4File.findMarkers(IMarker.PROBLEM, true, IResource.DEPTH_ZERO)[0]
							.getAttribute(IMarker.LINE_NUMBER, -1) == 3;
				} catch (Exception e) {
					return false;
				}
			}
		}.waitForCondition(editor.getEditorSite().getShell().getDisplay(), 30000);
		IMarker marker = p4File.findMarkers(IMarker.PROBLEM, true, IResource.DEPTH_ZERO)[0];
		assertTrue(marker.getType().contains("lsp4e"));
		assertEquals(3, marker.getAttribute(IMarker.LINE_NUMBER, -1));
	}
}
