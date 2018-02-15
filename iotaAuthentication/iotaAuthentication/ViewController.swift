//
//  ViewController.swift
//  iotaAuthentication
//
//  Created by Iciar on 10/2/18.
//  Copyright © 2018 Iciar. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    // MARK: Properties
    @IBOutlet weak var userNameTextField: UITextField!
    @IBOutlet weak var authenticationButton: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    // MARK: Actions
    @IBAction func clearUserName(_ sender: UIButton) {
        userNameTextField.text = ""
    }
    

    @IBAction func generateAuthenticationCode(_ sender: UIButton) {
        var code: UInt32
        
        code = arc4random()
        print(code)
        
    }
    
    @IBAction func sendPOSTAuthentication(_ sender: UIButton) {
        let myUrl = URL(string: "http://128.197.180.212/info.php");
        
        var request = URLRequest(url:myUrl!)
        
        request.httpMethod = "POST"// Compose a query string
        
        let postString = "id=12582912";
        
        request.httpBody = postString.data(using: String.Encoding.utf8);
        
        let task = URLSession.shared.dataTask(with: request) { (data: Data?, response: URLResponse?, error: Error?) in
            
            if error != nil
            {
                print("error=\(String(describing: error))")
                return
            }
            
            // Print out response object
            print("response = \(String(describing: response))")
            
            let responseString = String(data: data!, encoding: .utf8)
            print("responseString = \(responseString)")
            //Let's convert response sent from a server side script to a NSDictionary object:
            //do {
              //  let json = try JSONSerialization.jsonObject(with: data!, options: .mutableContainers) as? NSDictionary
                
                //if let parseJSON = json {
                    
                    // Now we can access value of First Name by its key
                  //  let firstNameValue = parseJSON["firstName"] as? String
                   // print("firstNameValue: \(String(describing: firstNameValue))")
                //}
            //} catch {
              //  print(error)
            //}
        }
        task.resume()
    }
}

