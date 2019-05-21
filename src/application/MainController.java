package application;

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.layout.BorderPane;
import javafx.stage.Stage;



public class MainController implements Initializable {
	@FXML private BorderPane firstPane;
	
	@FXML private void demoSequential(ActionEvent event) {}
	
	@FXML private void demoBinary(ActionEvent event) throws IOException {
		Parent parent = FXMLLoader.load(getClass().getResource("/main/resources/fxml/FXMLGraphicsPanel.fxml"));
		Scene newScene = new Scene(parent);
		Node node = (Node) event.getSource();
		Stage window = (Stage) node.getScene().getWindow();
		window.setScene(newScene);
		window.show();
		
	}
	
	@FXML private void demoAStar(ActionEvent event) {}
	
	@FXML private void exit(ActionEvent event) {
	    System.exit(0);
	}
	
	@Override
	public void initialize(URL arg0, ResourceBundle arg1) {
		// TODO Auto-generated method stub
		
	}
	
	
	
}
