from weakref import WeakKeyDictionary
from flask import Blueprint, render_template, request, flash, Response, jsonify
from flask_login import current_user
from werkzeug.utils import redirect
from .auth import user_privileges
from werkzeug.security import generate_password_hash
from .models import Vehicle, Stop, Link, Reservation, Route
from . import db
import sqlalchemy as sa
import requests
import random

operator = Blueprint('operator', __name__)

@operator.route('/manage_personnel', methods=['GET', 'POST'])
@user_privileges(user_role="operator") 
def manage_personnel_view():
    if request.method == 'POST':
        id =                request.form.get('text-id')
        login_name =        request.form.get('text-login-name')
        user_name =         request.form.get('text-personnel-name')
        user_surname =      request.form.get('text-personnel-surname')
        email =             request.form.get('text-email')
        password =          request.form.get('text-password')
        password_confirm =  request.form.get('text-password-confirm')
        phone_number =      request.form.get('text-phone-number')

        from .models import User

        personnel =             User.query.get(id)

        login_name_not_unique = (personnel.login_name != login_name) if personnel else True
        email_not_unique =      (personnel.email != email) if personnel else True

        # internal error, user with id @id does not exist, when id is empty, we create new personnel account
        if not personnel and id != "":
            return {'msg' : 'Internal server error', 'type' : 'danger'}
        # entered login name or email was not unique
        elif User.query.filter_by(login_name=login_name).first() and login_name_not_unique:
            return {'msg' : 'Account with this login name already exists', 'type' : 'danger'}
        elif User.query.filter_by(email=email).first() and email_not_unique:
            return {'msg' : 'Account with this email name already exists', 'type' : 'danger'}
        # testing that all fields are entered
        elif not login_name or not user_name or not user_surname or not email or not phone_number:
            return {'msg' : 'All fields must be filled out', 'type' : 'danger'}
        # Success: propagate changes to the database
        else:
            # create new personnel account
            if not personnel:
                # password integrity check
                if not password or not password_confirm:
                    return {'msg' : 'All fields must be filled out', 'type' : 'danger'}
                # passwords do not match
                elif password != password_confirm:
                    return {'msg' : 'Confirm password field isn\'t same as password', 'type' : 'danger'}
                # password too short
                elif len(password) < 5:
                    return {'msg' : 'Password must be at least 5 characters long', 'type' : 'danger'}
                # success
                else:
                    from .models import Role

                    personnel = User(
                        login_name =    login_name,
                        email =         email,
                        password =      generate_password_hash(password, method="sha256"),
                        user_name =     user_name,
                        user_surname =  user_surname,
                        phone_number =  phone_number
                    )
                    db.session.add(personnel)
                    passenger_role = Role.query.get("personnel")
                    # this code should be moved to pre-specified roles classifier
                    if passenger_role is None:
                        passenger_role = Role(name="personnel")
                    personnel.roles.append(passenger_role)

                    db.session.commit()

                    return {'msg' : 'Personnel account successfully created', 'type' : 'success', 'refresh' : True}

            # edit existing personnel        
            personnel.login_name =   login_name
            personnel.user_name =    user_name
            personnel.user_surname = user_surname
            personnel.email =        email
            personnel.phone_number = phone_number

            db.session.commit()
            return {'msg' : 'Personnel account successfully edited', 'type' : 'success', 'refresh' : True}
            
    return render_template('operator_views/manage_personnel_accounts.html')

@operator.route('/manage_personnel_data')
@user_privileges(user_role="operator") 
def manage_personnel_data():
    from .models import User
    # query personnel
    personnel_data = User.query.join(User.roles).filter_by(name='personnel').all()
    return jsonify({"data" : personnel_data})

# delete user
@operator.route('/delete_personnel', methods=["POST"])
@user_privileges(user_role="operator") 
def delete_users():
    from .models import User
    
    personnel = User.query.get(request.args['id'])
    if personnel:
        User.query.filter_by(id=request.args['id']).delete()
        db.session.commit()
        return {'msg' : 'Personnel account successfully deleted', 'type' : 'success'}
    else:    
        return {'msg' : 'Problem occurred while trying to delete personnel account', 'type' : 'danger'}


@operator.route('/design_infrastructure', methods = ["GET"])
@user_privileges(user_role="operator") 
def design_infrastructure_view():
    return render_template('operator_views/design_infrastructure.html')


# check data integrity and return dataset
def load_connection(connection_id):
    from .models import Link, User, Route
    connection = Link.query.get(connection_id)
    if not connection:
        return '<h2>Internal Server Error</h2>'

    operator = User.query.get(connection.operator)

    #if not operator:
    #    return '<h2>Internal Server Error</h2>'
    # conver interval to list
    interval = []
    for i in range(7):
        interval.append((connection.interval >> i) & 1)

    
    routes = Route.query.filter_by(link_id=connection_id).order_by(sa.asc(Route.arrival_time)).all()
    route_info = []
    for i, route in enumerate(routes):
        route_info.append((i, route.stop_id,route.arrival_time, route.departure_time))

    return render_template('operator_views/manage_connections.html', 
            connection_id = connection_id,
            connection_name = connection.link_name,
            operator_name = operator.login_name,
            interval = interval,
            license_plate = connection.vehicle.license_plate,
            capacity = connection.vehicle.first_class_capacity+connection.vehicle.second_class_capacity,
            routes = route_info)


def get_connection_form_data(request):
    form = request.form
    
    connection_name =       form.get('text-connection-name')
    operator_name =         form.get('text-operator-name')



    # connection name check
    if not connection_name:
        return { 'status' : 0x200, 'msg' : 'Connection name can\'t be empty', 'type' : 'danger', 'tab_id' : 'basic-info-tab' }
    
    from .models import User, Vehicle
    #DEBUGING VALUE
    operator_id = 3
    #operator_id = User.query.filter_by(login_name=operator_name).first()
    
    # logon operator check
    if not operator_name or not operator_id:
        return { 'status' : 'error', 'msg' : 'Internal server error - ( operator missing )', 'type' : 'danger' }

    # connection_day_interval
    interval =              0
    week =                  ['monday', 'tuesday', 'wednesday', 'thursday', 'friday', 'saturday', 'sunday']
    for day in week:
        value = form.get('checkbox-'+day) is not None
        interval |= (value << week.index(day))

    # at least one day in week needs to be selected
    if interval == 0:
        return { 'status' : 'error', 'msg' : 'Enter at least one day from the interval', 'type' : 'danger'}

    vehicle_license_plate = form.get('text-vehicle-license-plate')
    vehicle = Vehicle.query.get(vehicle_license_plate)

    # check for selected vehicle
    if not vehicle:
        return { 'status' : 'error', 'msg' : 'Choose vehicle for the connection', 'type' : 'danger'}

    # aggregate route data
    stops_info = []

    from datetime import time, timedelta, datetime
    
    i = 0
    while form.get('stop-name-'+str(i)) is not None:
        stop_name = form.get('stop-name-' + str(i))
        try:
            arrival_time = time.fromisoformat(form.get('stop-arrival-' + str(i)))
        # arrival time was not specified ( is required )
        except (ValueError, TypeError):
            return { 'status' : 'error', 'msg' : 'Every stop needs to have time specified', 'type' : 'danger'}
        # when departure time is not defined set by default [ arrival_time + minutes=2]
        try:
            departure_time = time.fromisoformat(form.get('stop-departure-' + str(i)))
        except (ValueError, TypeError):
            departure_time = timedelta(hours=arrival_time.hour, minutes=arrival_time.minute) + timedelta(minutes=2)
            departure_time = (datetime.min + departure_time).time()
        
        stops_info.append((stop_name, arrival_time, departure_time))
        i += 1

    # every connections requires at least 2 stops
    if len(stops_info) < 2:
        return { 'status' : 'error', 'msg' : 'Add at least two stops to the connection', 'type' : 'danger'}

    return { 
        'status'            : 'success',
        'connection_name'   : connection_name,
        'operator'          : operator_id,
        'interval'          : interval,
        'vehicle'           : vehicle,
        'routes'            : stops_info
        }


# loading and editing an existing connection
@operator.route('/manage_connections/<connection_id>', methods = ['GET', 'POST'])
@user_privileges(user_role="operator") 
def manage_connections_view(connection_id):
    if request.method == 'GET':
        return load_connection(connection_id)
    if request.method == 'POST':
        data = get_connection_form_data(request)
        
        # data integrity failed
        if data['status'] == 'error':
            return data

        from .models import Link, Route

        # created connection object
        connection = Link(
            confirmed =     False,
            link_name =     data['connection_name'],
            interval =      data['interval'],
            vehicle =       data['vehicle'],
            original_id =   connection_id,
            operator = current_user.id
        )
        db.session.add(connection)
        db.session.commit()

        # create and add routes
        for stop_name, arrival_time, departure_time in data['routes']:
            route = Route(
                link_id = connection.id,
                stop_id = stop_name,
                arrival_time = arrival_time,
                departure_time = departure_time
            )
            db.session.add(route)
        db.session.commit()

        return { 'status' : 'success', 'msg' : 'Connection edit request successfully created', 'type' : 'success' }


@operator.route('/manage_connections', methods= ['GET', 'POST'])
@user_privileges(user_role="operator") 
def manage_new_connection_view():
    if request.method == 'POST':
        data = get_connection_form_data(request)
        
        # data integrity failed
        if data['status'] == 'error':
            return data

        from .models import Link, Route

        # created connection object
        connection = Link(
            confirmed = False,
            operator =  data['operator'],
            link_name = data['connection_name'],
            interval =  data['interval'],
            vehicle =   data['vehicle']
        )
        db.session.add(connection)
        db.session.commit()

        # create and add routes
        for stop_name, arrival_time, departure_time in data['routes']:
            route = Route(
                link_id = connection.id,
                stop_id = stop_name,
                arrival_time = arrival_time,
                departure_time = departure_time
            )
            db.session.add(route)
        db.session.commit()

        return { 'status' : 'success', 'msg' : 'Connection successfully created', 'type' : 'success'}
        
    if not current_user.is_authenticated:
        return { 'status' : 'error', 'msg' : 'Connection successfully created', 'type' : 'danger'}

    return render_template('operator_views/manage_connections.html', operator_name=current_user.user_name, operator_id=current_user.id)


@operator.route('/list_connections', methods=['GET'])
@user_privileges(user_role="operator") 
def get_connection_data():
    from .models import Link
    links = Link.query.filter_by(confirmed=True).all()
    return jsonify({ 'data' : links })


@operator.route('/connections')
@user_privileges(user_role="operator")
def connections_view():
    return render_template('operator_views/list_connections.html')


@operator.route('/connections_redirect_edit')
def connections_redirect_edit():
    return redirect('operator_views/manage_connections.html')

@operator.route('/manage_insfrastructure', methods = ["GET","POST"])
def manage_insfrastructure_view():
    return render_template('')

@operator.route('/stops_data')
def get_stops_data():
    status = request.args['status']
    if status:
        stops = Stop.query.filter_by(status=status).all()
        return jsonify({ 'data' : stops})
    return ''

@operator.route('/get_stops', methods = ["GET", "POST"])
@user_privileges(user_role="operator") 
def stops2json():
    if request.form.get("status"):
        data = Stop.query.filter_by(status=request.form.get("status")).all()
        return jsonify({ 'data' : data })
    else:
        #TODO
        return



@operator.route('/save_stop', methods = ["POST"])
@user_privileges(user_role="operator") 
def save_stop():
    if request.method == 'POST':
        name = request.form.get('name')
        address = request.form.get('address')
        lat = request.form.get('latitude')
        lng = request.form.get('longitude')
        description = request.form.get('description')
        query_stop = Stop.query.filter_by(name=address).first()

        if query_stop:
            return Response(render_template('operator_views/design_infrastructure.html'), status=210)
        elif not address or not lat or not lng or not description:
            return Response(render_template('operator_views/design_infrastructure.html'), status=211)
        else:
            stop = Stop(name=name, address=address, latitude=lat, longitude=lng, description=description, status=0)
            db.session.add(stop)
            db.session.commit()
            return Response(render_template('operator_views/design_infrastructure.html'), status=200)


@operator.route('/calculate_coordinates', methods = ["POST"])
@user_privileges(user_role="operator") 
def calculate_position():
    if request.method == 'POST':
        address = request.form.get('address')
        key = "3TzvxYRooQ62QNm8nbd0dsISCiAEkxUk"
        url = "https://www.mapquestapi.com/geocoding/v1/address?outFormat=json&key=" + key + "&location=" + address
        #mapurl = "https://www.mapquestapi.com/staticmap/v5/map?key=" + key +"&center="+ address + ",MA&size=600,400@2x"
        try:
            response = requests.get(url)
        except:
            return False
        data = response.json()
        if data['results'][0]['locations'][0]['latLng']['lat']:
            lat = data['results'][0]['locations'][0]['latLng']['lat']
        if data['results'][0]['locations'][0]['latLng']['lng']:
            lng = data['results'][0]['locations'][0]['latLng']['lng']
        if data['results'][0]['locations'][0]['mapUrl']:
            map = data['results'][0]['locations'][0]['mapUrl']
        if lat and lng and map:
            values = {'latitude' : lat, 'longitude' : lng, 'map' : map}
            return jsonify(values)
    else:
        return render_template('operator_views/design_infrastructure.html')


@operator.route('/add_vehicle', methods = ['POST'])
@user_privileges(user_role="operator") 
def add_vehicle():
    if request.method == 'POST':
        license_plate =     request.form.get('add-license_plate')
        vehicle_type =      request.form.get('add-type')
        first_c_capacity =  request.form.get('add-1st-capacity')
        second_c_capacity = request.form.get('add-2nd-capacity')
        description =       request.form.get('add-description')

        query_vehicle = Vehicle.query.filter_by(license_plate=license_plate).first()
        #vehicle with same license plate already exists
        if query_vehicle:
            return Response(render_template('operator_views/manage_vehicles.html'), status=210)
        # license plate not specified
        elif not license_plate:
            return Response(render_template('operator_views/manage_vehicles.html'), status=211)
        else:
            vehicle = Vehicle(
                license_plate=license_plate,
                first_class_capacity=first_c_capacity,
                second_class_capacity=second_c_capacity,
                vehicle_type=vehicle_type,
                description=description
            )
            db.session.add(vehicle)
            db.session.commit()
            return Response(render_template('operator_views/manage_vehicles.html'), status=200)
    else:
        return render_template('operator_views/manage_vehicles.html')


@operator.route('/delete_vehicle', methods=["GET"])
@user_privileges(user_role="operator") 
def delete_vehicle():
    if request.method == 'GET':
        if not request.args['license_plate']:
            return Response(render_template('operator_views/manage_vehicles.html'), status=210)
        else:
            links = Link.query.filter_by(vehicle_id=request.args['license_plate']).all()
            for link in links:
                Reservation.query.filter_by(connection_id=link.id).delete()
                Route.query.filter_by(link_id=link.id).delete()
                db.session.delete(link)
                db.session.commit()
            Vehicle.query.filter_by(license_plate=request.args['license_plate']).delete()
            db.session.commit()
            return Response(render_template('operator_views/manage_vehicles.html'), status=200)

@operator.route('/delete_connection/<id>', methods=["GET"])
@user_privileges(user_role="operator")
def delete_connection(id):
    id = int(id)
    from .models import Link, Route, Reservation
    # renmove route from link
    Route.query.filter_by(link_id = id).delete()
    db.session.commit()
    # remove reservations from link
    Reservation.query.filter_by(connection_id=id)
    db.session.commit()
    link = Link.query.filter_by(id=id).first()
    link.operator = None
    link.original_id = None
    link.vehicle_id = None
    db.session.delete(link)
    db.session.commit()
    return Response(render_template('operator_views/list_connections.html'), status=200)

@operator.route('/create_edit_request', methods = ['POST'])
@user_privileges(user_role="operator") 
def create_edit_request():
    if request.method == 'POST':
        name = request.form.get('edit-name')
        new_address = request.form.get('edit-new-address')
        latitude = request.form.get('edit-latitude')
        longitude = request.form.get('edit-longitude')
        description = request.form.get('edit-description')
        from .models import Stop
        if not new_address or not latitude or not longitude or not description:
            return Response(render_template('operator_views/design_infrastructure.html'), status=210)
        else:
            existence = Stop.query.filter_by(address=new_address).first()
            if existence:
                return Response(render_template('operator_views/design_infrastructure.html'), status=211)
            else:
                current_stop = Stop.query.filter_by(name=name).first()
                request_name = str("Edit request-" + current_stop.name)
                request_copy = Stop(name=request_name,address=new_address,latitude=latitude, longitude = longitude, status = 2, description=description)
                db.session.add(request_copy)
                db.session.commit()
                return Response(render_template('operator_views/design_infrastructure.html'), status=200)

@operator.route('/request_delete_stop', methods=["POST"])
@user_privileges(user_role="operator") 
def request_delete_stop():
    if request.method == "POST":
        if not request.args['name']:
            return Response(render_template('operator_views/design_infrastructure.html'), status=210)
        else:
            stop = Stop.query.filter_by(name=request.args['name']).first()
            request_copy = Stop(name=str("Delete request-"+ request.args['name']), address=stop.address, latitude=stop.latitude, longitude = stop.longitude, status = 3, description=stop.description)
            db.session.add(request_copy)
            db.session.commit()
            return Response(render_template('operator_views/design_infrastructure.html'), status=200)

@operator.route('/manage_vehicles2json')
@user_privileges(user_role="operator") 
def manage_vehicle_2json():
    data = Vehicle.query.all()
    if not data:
        return jsonify({'data': ""})
    return jsonify({ 'data' : data })


@operator.route('/edit_vehicles', methods=['POST'])
@user_privileges(user_role="operator") 
def edit_vehicles():
    license_plate = request.form.get('edit-license_plate')
    first_c_capacity = int(request.form.get('edit-first-capacity'))
    second_c_capacity = int(request.form.get('edit-second-capacity'))
    veh_type = request.form.get('edit-vehicle-type')
    description = request.form.get('edit-description')
    vehicle =  Vehicle.query.filter_by(license_plate=license_plate).first()
    
    if not vehicle:
        return Response(render_template('operator_views/manage_vehicles.html'), status=210)
    elif not description:
        return Response(render_template('operator_views/manage_vehicles.html'), status=212)
    # same values
    elif veh_type == vehicle.vehicle_type and first_c_capacity == vehicle.first_class_capacity and description == vehicle.description and second_c_capacity == vehicle.second_class_capacity:
        return Response(render_template('operator_views/manage_vehicles.html'), status=211)
    else:
        vehicle.first_class_capacity = first_c_capacity
        vehicle.second_class_capacity = second_c_capacity
        vehicle.vehicle_type = veh_type
        vehicle.description = description
        db.session.commit()
        flash('Edit successfull', category='success')
        return Response(render_template( 'admin_views/manage_user_accounts.html'), status=200)
    

@operator.route('/manage_vehicles', methods=['GET'])
@user_privileges(user_role="operator") 
def manage_vehicles_view():
    return render_template('operator_views/manage_vehicles.html')