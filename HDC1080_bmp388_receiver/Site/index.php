<?php
if($_POST){
	define("DATABASE_CONNECT_HOST", "HOST");     			// Host para conexão com o banco de dados
	define("DATABASE_CONNECT_USER", "USUARIO_BD");    // Usuário para conexão com o banco de dados
	define("DATABASE_CONNECT_PASSWORD", "SENHA_BD");  // Senha para conexão com o banco de dados
	define("DATABASE_CONNECT_DATABASE", "NOME_BD");   // Nome do banco de dados

	$mysqli = new mysqli(DATABASE_CONNECT_HOST, DATABASE_CONNECT_USER, DATABASE_CONNECT_PASSWORD, DATABASE_CONNECT_DATABASE);

	if (mysqli_connect_errno()) {
		echo '<div style="position: absolute;left: 0;right: 0;z-index: 30;background: #ff9999;padding: 20px; text-align: center;">';
		printf("Falha ao conectar: %s\n", mysqli_connect_error());
		echo '</div>';
	}

	$GLOBALS['mysqli'] = $mysqli;

	$queryStr = "
		INSERT INTO loraweather 
			(id, bmp_pressure, bmp_temperature, bmp_altitude, hdc1080_temperature, hdc1080_humidity) 
		VALUES 
			(
				NULL, 
				'".$_POST['bmp_pressure']."', 
				'".$_POST['bmp_temperature']."', 
				'".$_POST['bmp_altitude']."', 
				'".$_POST['hdc1080_temperature']."', 
				'".$_POST['hdc1080_humidity']."'
			)";

	$execute_query = $mysqli->query($queryStr);

	if($execute_query){
		echo 'inserido com sucesso';
	}else{
		echo 'erro ao inserir <br>';
		echo $mysqli->error;
	}
}
?>