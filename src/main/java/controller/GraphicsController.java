package main.java.controller;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.stage.Stage;

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;

/**
 * Constructs the GUI components and performs events for displaying and
 * manipulating the binary tree.
 * @author Eric Canull
 * @version 1.0
 */
public final class GraphicsController implements Initializable {

	// Panels and other GUI components
	@FXML private BorderPane root_container;
	@FXML private TextArea traversal_textarea;
	@FXML private TextField input_field;

	private GraphicsTree graphicsTree;

	/**
	 * Constructs the GUI components and performs events for displaying and
	 * changing the data in the binary tree.
	 */
	@Override
	public void initialize(URL location, ResourceBundle resources) {

		// The center panel for drawing the tree
		graphicsTree = new GraphicsTree();
		// Add the panels onto the border pane
		root_container.setCenter(graphicsTree);

		// Bind canvas size to stack pane size.
		graphicsTree.widthProperty().bind(root_container.widthProperty());
		graphicsTree.heightProperty().bind(root_container.heightProperty().subtract(50));
	}
	
	/**
	 *  Performs the action when the search button is clicked.
	 */
	@FXML private void searchOnAction(ActionEvent event) {
		try {
			graphicsTree.search(Integer.parseInt(input_field.getText().trim()));
		} catch (NumberFormatException nfe) {
			Alert alert = new Alert(Alert.AlertType.ERROR,
					"Error searching for value. The input field can only accept numbers.", 	ButtonType.OK);

			alert.showAndWait()
					.filter(response -> response == ButtonType.OK)
					.ifPresent(response -> alert.close());

		}
	}

	/**
	 * Performs the action when the delete button is clicked.
	 */
	@FXML private void deleteOnAction(ActionEvent event) {
		try {
			graphicsTree.delete(Integer.parseInt(input_field.getText().trim()));
		} catch (NumberFormatException nfe) {
			Alert alert = new Alert(Alert.AlertType.ERROR, "Error deleting value. The input field can only accept numbers.",
					ButtonType.OK);
			alert.showAndWait()
					.filter(response -> response == ButtonType.OK)
					.ifPresent(response -> alert.close());
		}
	}

	private void clearTree() {
		graphicsTree.makeEmpty();
		traversal_textarea.setText("");
	}
	/**
	 * Performs the action when the clear button is clicked.
	 */
	@FXML private void clearOnAction(ActionEvent event) {
		Alert alert = new Alert(Alert.AlertType.CONFIRMATION, "Do you want to empty the tree?", ButtonType.OK);
		alert.showAndWait()
				.filter(response -> response == ButtonType.OK)
				.ifPresent(response -> clearTree());

	}

	/**
	 *  Performs the action when the insert button is clicked.
	 */
	@FXML private void insertOnAction(ActionEvent event) {
		try {
			graphicsTree.insert(Integer.parseInt(input_field.getText().trim()));
		} catch (NumberFormatException nfe) {
			Alert alert = new Alert(Alert.AlertType.ERROR, "Error inserting value. The input field can only accept numbers.",
					ButtonType.OK);
			alert.showAndWait()
					.filter(response -> response == ButtonType.OK)
					.ifPresent(response -> alert.close());
		}
	}

	
	@FXML private void postorderOnAction(ActionEvent event) throws Exception {
		Parent parent = FXMLLoader.load(getClass().getResource("/application/FXMLMain.fxml"));
		Scene newScene = new Scene(parent);
		Node node = (Node) event.getSource();
		Stage window = (Stage) node.getScene().getWindow();
		window.setScene(newScene);
		window.show();
	}
}
