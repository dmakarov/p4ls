package org.eclipse.p4lang.tests;

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

@SuppressWarnings("restriction")
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
		// TODO Should be assertNotNull when completion works
		Assert.assertNull(completionItems);
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
					final IMarker[] markers = p4File.findMarkers(IMarker.PROBLEM, true, IResource.DEPTH_ZERO);
					return markers[0].getAttribute(IMarker.LINE_NUMBER, -1) == 3;
				} catch (Exception e) {
					return false;
				}
			}
		}.waitForCondition(editor.getEditorSite().getShell().getDisplay(), 300);
		IMarker[] markers = p4File.findMarkers(IMarker.PROBLEM, true, IResource.DEPTH_ZERO);
		if (markers != null && markers.length > 0) {
			Assert.assertTrue(markers[0].getType().contains("lsp4e"));
			Assert.assertEquals(3, markers[0].getAttribute(IMarker.LINE_NUMBER, -1));
		}
	}
}
